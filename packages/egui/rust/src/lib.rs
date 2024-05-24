#![warn(clippy::all, rust_2018_idioms)]

mod app;

use std::collections::HashMap;
use std::ops::{Deref};
use std::sync::{Arc, Mutex};
use eframe::Frame;
use egui::{Context};
use once_cell::sync::Lazy;
use serde::{Deserialize, Serialize};
use wasm_bindgen::prelude::*;
use serde_json::{Value};
use erased_serde::serialize_trait_object;
use js_sys::Function;

type Widgets = HashMap<u32, Box<dyn Render + Send>>;
type Hierarchy = HashMap<u32, Vec<u32>>;

pub static WIDGETS: Lazy<Arc<Mutex<Widgets>>> = Lazy::new(|| {
    Arc::new(Mutex::new(HashMap::new()))
});

pub static HIERARCHY: Lazy<Arc<Mutex<Hierarchy>>> = Lazy::new(|| {
    Arc::new(Mutex::new(HashMap::new()))
});

#[wasm_bindgen]
extern "C" {
    #[wasm_bindgen(js_namespace = console)]
    fn log(a: &str);
}

pub struct EventHandlers {
    on_click: JsValue,
    on_text_change: JsValue,
    on_bool_value_change: JsValue
}

impl EventHandlers {
    pub fn new(on_click: JsValue, on_text_change: JsValue, on_bool_value_change: JsValue) -> EventHandlers {
        EventHandlers{
            on_click,
            on_text_change,
            on_bool_value_change
        }
    }

    pub fn on_click(&self, id: u32) {
        self.on_click.unchecked_ref::<Function>().call1(&JsValue::NULL, &JsValue::from(id)).unwrap();
    }

    pub fn on_text_change(&self, id: u32, value: String) {
        self.on_text_change.unchecked_ref::<Function>().call2(&JsValue::NULL, &JsValue::from(id), &JsValue::from(value)).unwrap();
    }

    pub fn on_bool_value_change(&self, id: u32, value: bool) {
        self.on_bool_value_change.unchecked_ref::<Function>().call2(&JsValue::NULL, &JsValue::from(id), &JsValue::from(value)).unwrap();
    }
}

#[wasm_bindgen]
pub fn init_egui(on_click: JsValue, on_text_change: JsValue, on_bool_value_change: JsValue) {
    // Redirect `log` message to `console.log` and friends:
    eframe::WebLogger::init(log::LevelFilter::Debug).ok();

    let web_options = eframe::WebOptions::default();

    wasm_bindgen_futures::spawn_local(async {
        eframe::WebRunner::new()
            .start(
                "the_canvas_id", // hardcode it
                web_options,
                Box::new(|cc| Box::new(
                    crate::App::new(
                        EventHandlers::new(on_click, on_text_change, on_bool_value_change),
                        cc
                        )
                    )
                ),
            )
            .await
            .expect("failed to start eframe");
    });
}

pub struct App {
    event_handlers: EventHandlers
}

impl App {
    pub fn new(event_handlers: EventHandlers, _cc: &eframe::CreationContext<'_>) -> App {
        App{ event_handlers }
    }

    pub fn render_widget_by_id(&mut self, widgets: &mut Widgets, hierarchy: &Hierarchy, ui: &mut egui::Ui, id: u32) {
        if widgets.contains_key(&id) {
            ui.push_id(id, |ui| {
                match widgets.get(&id).unwrap_throw().get_type() {
                    "Horizontal" => {
                        ui.horizontal(|ui| {
                            self.render_children(widgets, hierarchy, ui, id);
                        });
                    }
                    "CollapsingHeader" => {
                        // todo: not sure which of the two approaches to take, bearing in mind that CollapsingState is the most flexible of the two
                        // egui::CollapsingHeader::new(widgets.get(&id).unwrap_throw().get_label())
                        //     .enabled(false)
                        //     .show(ui, |ui| {
                        //         self.render_children(widgets, hierarchy, ui, id);
                        //     });

                        let collapsing_header_id = ui.make_persistent_id(id);
                        let collapsing_state = egui::collapsing_header::CollapsingState::load_with_default_open(ui.ctx(), collapsing_header_id, true);

                        collapsing_state.show_header(ui, |ui| {
                                ui.label(widgets.get(&id).unwrap_throw().get_label()).clicked(); // you can put checkboxes or whatever here
                            })
                            .body(|ui| {
                                self.render_children(widgets, hierarchy, ui, id);
                            })
                        ;
                    }
                    &_ => {
                        widgets.get_mut(&id).unwrap().render(ui, self);
                    }
                }
            });
        }
    }

    pub fn render_widgets(&mut self, widgets: &mut Widgets, hierarchy: &Hierarchy, ui: &mut egui::Ui, id: Option<u32>) {
        let normalized_id = id.or(Some(0)).unwrap();

        if widgets.get(&normalized_id).is_some() {
            self.render_widget_by_id(widgets, hierarchy, ui, normalized_id);
        }

        if widgets.get(&normalized_id).is_none() {
            // render_children?
            self.render_children(widgets, hierarchy, ui, normalized_id);
        }
    }

    pub fn render_children(&mut self, w: &mut Widgets, hierarchy: &Hierarchy, ui: &mut egui::Ui, id: u32) {
        if hierarchy.get(&id).is_some() {
            let children_ids = hierarchy.get(&id).unwrap();

            if !children_ids.is_empty() {
                for val in children_ids.iter() {
                    self.render_widgets(w, hierarchy, ui, Some(*val));
                }
            }
        }
    }
}

impl eframe::App for App {
    fn update(&mut self, ctx: &Context, _frame: &mut Frame) {
        let mut widgets = WIDGETS.lock().unwrap_throw();
        let hierarchy = HIERARCHY.lock().unwrap_throw();

        egui::CentralPanel::default().show(ctx, |ui| {
            self.render_widgets(&mut widgets, hierarchy.deref(), ui, None);
        });
    }
}

#[wasm_bindgen]
pub fn append_child(parent_id: u32, child_id: u32) -> () {
    let mut m = HIERARCHY.lock().unwrap_throw();

    let children = m.get_mut(&parent_id);

    if children.is_some() {
        let vec = children.unwrap();

        if !vec.contains(&child_id) {
            vec.push(child_id);
        }
    } else {
        m.insert(parent_id, vec![child_id]);
    }
}

#[wasm_bindgen]
pub fn set_children(parent_id: u32, raw_children_ids: String) -> () {
    let mut m = HIERARCHY.lock().unwrap_throw();

    let children_ids: Value = serde_json::from_str(&*raw_children_ids).unwrap();

    if children_ids.is_array() {
        // Convert array of JSON Value instances into Vec<u32>
        m.insert(parent_id, children_ids.as_array().unwrap().iter().map(|x| x.as_u64().unwrap() as u32).collect());
    }
}

#[wasm_bindgen]
pub fn get_widgets() -> String {
    let m = WIDGETS.lock().unwrap_throw();

    return serde_json::to_string(&m.deref()).unwrap();
}

#[wasm_bindgen]
pub fn get_hierarchy() -> String {
    let h = HIERARCHY.lock().unwrap_throw();

    return serde_json::to_string(&h.deref()).unwrap();
}

#[wasm_bindgen]
pub fn set_widget(raw_widget_def: String) {
    let mut m = WIDGETS.lock().unwrap_throw();

    let widget_def: Value = serde_json::from_str(&*raw_widget_def).unwrap();

    if widget_def.is_object() && widget_def["type"].is_string() {
        let maybe_widget_type = widget_def["type"].as_str();
        let maybe_widget_id = widget_def["id"].as_u64();

        if maybe_widget_id.is_some() && maybe_widget_type.is_some() {
            let widget_id = maybe_widget_id.unwrap() as u32;
            let widget_type = maybe_widget_type.unwrap();

            match widget_type {
                "Button" => {
                    if widget_def["label"].is_string() {
                        m.insert(widget_id, Box::new(Button::new(widget_id, widget_def["label"].as_str().unwrap())));
                    }
                }
                "InputText" => {
                    let default_value = widget_def["defaultValue"].as_str();

                    if default_value.is_some() {
                        m.insert(widget_id, Box::new(InputText::new(widget_id, default_value)));
                    }
                }
                "Checkbox" => {
                    let label = widget_def["label"].as_str();
                    let tooltip_text = widget_def["tooltipText"].as_str();
                    let default_checked = widget_def["defaultChecked"].as_bool();

                    log(tooltip_text.unwrap_or_default());

                    if label.is_some() {
                        m.insert(widget_id, Box::new(Checkbox::new(
                            widget_id,
                            label.unwrap(),
                            default_checked.or(Some(false)).unwrap(),
                            tooltip_text))
                        );
                    }
                }
                "Horizontal" => {
                    m.insert(widget_id, Box::new(Horizontal::new(widget_id)));
                }
                "CollapsingHeader" => {
                    if widget_def["label"].is_string() {
                        m.insert(widget_id, Box::new(CollapsingHeader::new(widget_id, widget_def["label"].as_str().unwrap())));
                    }
                }
                &_ => {
                    log(format!("Unrecognised type: {}", widget_type).as_str());
                }
            }
        }
    }
}

// ----------------

pub trait Render: erased_serde::Serialize {
    fn render(&mut self, ui: &mut egui::Ui, app: &App);

    fn get_type(&self) -> &str;

    fn get_label(&self) -> &str;
}

#[derive(Serialize, Deserialize)]
pub struct Button {
    pub id: u32,
    pub label: String,
    pub widget_type: String
}
//
impl Button {
    pub fn new(id: u32, label: &str) -> Button {
        Button{
            id,
            label: String::from(label),
            widget_type: String::from("Button")
        }
    }
}
//
impl Render for Button {
    fn render(&mut self, ui: &mut egui::Ui, app: &App) {
        if ui.button(self.label.as_str()).clicked() {
            app.event_handlers.on_click(self.id);
        }
    }

    fn get_type(&self) -> &str {
        return self.widget_type.as_str();
    }

    fn get_label(&self) -> &str {
        return "";
    }
}

#[derive(Serialize, Deserialize)]
pub struct InputText {
    pub id: u32,
    pub value: String,
    pub widget_type: String
}

impl InputText {
    fn new(id: u32, value: Option<&str>) -> InputText {
        InputText{
            id,
            value: String::from(value.unwrap_or_default()),
            widget_type: String::from("InputText")
        }
    }
    fn set_value(mut self, value: &str) -> () {
        self.value = String::from(value);
    }
}

impl Render for InputText {
    fn render(&mut self, ui: &mut egui::Ui, app: &App) {
        if ui.text_edit_singleline(&mut self.value).changed() {
            app.event_handlers.on_text_change(self.id, self.value.clone());
        }
    }

    fn get_type(&self) -> &str {
        return self.widget_type.as_str();
    }

    fn get_label(&self) -> &str {
        return "";
    }
}

#[derive(Serialize, Deserialize)]
pub struct Checkbox {
    pub id: u32,
    pub checked: bool,
    pub label: String,
    pub tooltip_text: Option<String>,
    pub widget_type: String
}

impl Checkbox {
    fn new(id: u32, label: &str, checked: bool, tooltip_text: Option<&str>) -> Checkbox {
        Checkbox {
            id,
            checked,
            label: String::from(label),
            widget_type: String::from("Checkbox"),
            tooltip_text: if tooltip_text.is_some() { Some(String::from(tooltip_text.unwrap())) } else { None }
        }
    }
    fn set_checked(mut self, checked: bool) -> () {
        self.checked = checked;
    }
}

impl Render for Checkbox {
    fn render(&mut self, ui: &mut egui::Ui, app: &App) {
        let response = ui.checkbox(&mut self.checked, self.label.as_str());

        if self.tooltip_text.is_some() {
            response.on_hover_text(self.tooltip_text.as_ref().unwrap());
        }
    }

    fn get_type(&self) -> &str {
        return self.widget_type.as_str();
    }

    fn get_label(&self) -> &str {
        return self.label.as_str();
    }
}

#[derive(Serialize, Deserialize)]
struct Horizontal {
    pub id: u32,
    pub widget_type: String
}

impl Horizontal {
    fn new(id: u32) -> Horizontal {
        Horizontal{
            id,
            widget_type: String::from("Horizontal")
        }
    }
}

impl Render for Horizontal {
    fn render(&mut self, _ui: &mut egui::Ui, app: &App) {}

    fn get_type(&self) -> &str { return self.widget_type.as_str(); }

    fn get_label(&self) -> &str {
        return "";
    }
}

#[derive(Serialize, Deserialize)]
struct CollapsingHeader {
    pub id: u32,
    pub label: String,
    pub widget_type: String
}

impl CollapsingHeader {
    pub fn new(id: u32, label: &str) -> CollapsingHeader {
        CollapsingHeader{
            id,
            label: String::from(label),
            widget_type: String::from("CollapsingHeader")
        }
    }
}

impl Render for CollapsingHeader {
    fn render(&mut self, _ui: &mut egui::Ui, app: &App) {}

    fn get_type(&self) -> &str {
        return self.widget_type.as_str();
    }

    fn get_label(&self) -> &str {
        return &self.label;
    }
}

serialize_trait_object!(Render);