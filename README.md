# React WASM GUI bindings

## Motivation

I've always found WebAssembly very interesting conceptually but I never really needed to use it - until relatively recently.

I invested quite a bit of time and effort into figuring out how [WebAssembly](https://webassembly.org/) and [Emscripten](https://emscripten.org/index.html) work. I eventually managed to compile the [Dear ImGui](https://github.com/ocornut/imgui) emscripten example and modified it to suit my requirements. Given my lack of experience with C/C++, I can tell you that this has been far from being a straightforward process. I have eventually realized that someone else out there might benefit from this experience of mine.

As I write these paragraphs, I realise that, despite accomplishing a few small initial goals, there is still lot to do. I hope you find these libraries useful, if anything just to get yourself acquainted with WebAssembly, C/C++ (and soon Rust).

## Caveats

### Overall quality

I work on this project during my spare time so the overall quality is ... quite poor. Please bear with me while I make the necessary improvements.

Due of my very limited expertise with C/C++, it is highly likely that the code I wrote so far is buggy and not particularly performant (read, quite crappy).

There are currently no tests. I would like to add coverage for the C/C++ layer ASAP.

### Performance

Performance-wise, the current implementation of the libraries is far from being production-ready. In general the code can be optimised but I also have a growing suspicion that they could benefit from multi-threading. That said, I have learned that, for example, ImGui cannot take control of an off-screen Canvas instance due to its attempts to resize it. Also, attempts to enable the `-sPROXY_TO_PTHREAD` emscripten switch proved unsuccessful as ultimately WebGL instructions can be sent only from the main thread. I believe it is possible to leverage multi-threading in an emscripten-compatible way though I have yet to figure that one out.

### Accessibility

GUI libraries such as [egui](https://github.com/emilk/egui) are a very good example of accessible non-DOM based GUIs. The overall impression I get is that DOM-based GUIs are more accessible. Perhaps this project will foster interest in the topic and motivate people and/or companies to invest more in this area.

### Support for other frameworks

At the moment I am focusing on bindings for React only. The renderer is actually adapted from react-native's Fabric renderer.
Perhaps there are other options I could/should have considered. Feel free to let me know your thoughts.

## Bindings

### Dear Imgui

[Dear Imgui](https://github.com/andreamancuso/react-wasm/tree/main/packages/dear-imgui) (work in progress) Browser support: latest Chrome and Edge only presently.

![React Dear Imgui screenshot 1](/screenshots/dear-imgui/react-wasm-dear-imgui-s1.png?raw=true)

![React Dear Imgui screenshot 2](/screenshots/dear-imgui/react-wasm-dear-imgui-s0.png?raw=true)

![React Dear Imgui screenshot 3](/screenshots/dear-imgui/react-wasm-dear-imgui-s2.png?raw=true)

![React Dear Imgui screenshot 4](/screenshots/dear-imgui/screenshot-react-wasm-dear-imgui-sample-code.png?raw=true)

![React Dear Imgui Electron demo](/screenshots/dear-imgui/electron-demo.png?raw=true)

### egui

-   [egui](https://github.com/andreamancuso/react-wasm/tree/main/packages/egui) (work in progress) Browser support: Chrome, Firefox, Edge

![egui hello world](/screenshots/egui/egui-hello-world.png?raw=true)

## Other/future bindings

Would you like to help add support for a GUI library of your choice? Let's discuss.
