# Dockable Windows Implementation Guide
## Future Enhancement for Landscape Designer

## Current Status
✅ **Bug fixes implemented**:
- Fixed soft clamp mode safety for small height ranges
- Added warning when engine presets create narrow ranges  
- Engine presets now calculate range without auto-enabling clamping
- Improved user guidance with helpful tips

⚠️ **Dockable windows NOT implemented yet** because:
- vcpkg's ImGui is **standard branch** (no docking support)
- Docking requires **ImGui docking branch** or version ≥1.80

## How to Upgrade to Dockable Windows

### Option 1: Switch to ImGui Docking Branch (Recommended)

#### Step 1: Update vcpkg to docking branch
```powershell
# Uninstall current ImGui
vcpkg remove imgui

# Install docking branch
vcpkg install imgui[docking-experimental]

# Or manually clone docking branch
cd vcpkg/ports/imgui
git clone https://github.com/ocornut/imgui.git --branch docking
```

#### Step 2: Enable docking in main.cpp
```cpp
// In main() after ImGui::CreateContext()
ImGuiIO& io = ImGui::GetIO();
io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Multi-monitor support

// Style tweaks for docking
ImGuiStyle& style = ImGui::GetStyle();
if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
}
```

#### Step 3: Create Dockspace
```cpp
// In render loop
ImGui_ImplOpenGL3_NewFrame();
ImGui_ImplGlfw_NewFrame();
ImGui::NewFrame();

// Create fullscreen dockspace
ImGuiViewport* viewport = ImGui::GetMainViewport();
ImGui::SetNextWindowPos(viewport->WorkPos);
ImGui::SetNextWindowSize(viewport->WorkSize);
ImGui::SetNextWindowViewport(viewport->ID);

ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGui WindowFlags_NoCollapse;
windowFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
ImGui::Begin("DockSpaceWindow", nullptr, windowFlags);
ImGui::PopStyleVar();

// Create dockspace
ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

// Menu bar
if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("Windows")) {
        ImGui::MenuItem("2D Paint View", nullptr, &show2DView);
        ImGui::MenuItem("3D Preview", nullptr, &show3DView);
        ImGui::MenuItem("Controls", nullptr, &showControls);
        ImGui::MenuItem("Statistics", nullptr, &showStats);
        ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
}

ImGui::End(); // DockSpaceWindow
```

#### Step 4: Create Dockable Windows
```cpp
// 2D Paint View
if (show2DView) {
    ImGui::Begin("2D Paint View", &show2DView);
    ImVec2 size = ImGui::GetContentRegionAvail();
    
    // Display heightmap texture
    ImGui::Image((void*)(intptr_t)renderer.getTextureID(), size, 
                ImVec2(0, 1), ImVec2(1, 0));
    
    // Handle painting when hovered
    if (ImGui::IsItemHovered() && params.paintMode) {
        // Convert mouse position to heightmap coordinates
        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 itemMin = ImGui::GetItemRectMin();
        ImVec2 relPos = ImVec2(mousePos.x - itemMin.x, mousePos.y - itemMin.y);
        
        float normX = relPos.x / size.x;
        float normY = 1.0f - (relPos.y / size.y);
        
        int hx = (int)(normX * rawHeight.width());
        int hy = (int)(normY * rawHeight.height());
        
        // Paint on left click
        if (ImGui::IsMouseDown(0)) {
            brushTool.applyBrush(rawHeight, hx, hy, 
                                params.brushType, 
                                params.brushRadius,
                                params.brushStrength,
                                params);
            renderer.uploadTexture(rawHeight);
        }
        
        // Draw brush cursor
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImVec2 center = ImVec2(itemMin.x + relPos.x, itemMin.y + relPos.y);
        float screenRadius = (params.brushRadius / params.mapResolution) * size.x;
        drawList->AddCircle(center, screenRadius, IM_COL32(255, 255, 0, 200), 32, 2.0f);
    }
    
    ImGui::End();
}

// 3D Preview Window
if (show3DView) {
    ImGui::Begin("3D Preview", &show3DView);
    
    ImGui::SliderFloat("Height Scale", &params.heightScale, 0.1f, 2.0f);
    if (ImGui::Button("Reset Camera")) {
        camera.reset();
    }
    
    ImGui::Separator();
    ImGui::Text("Distance: %.2f", camera.getDistance());
    ImGui::Text("Yaw: %.1f", camera.getYaw());
    ImGui::Text("Pitch: %.1f", camera.getPitch());
    
    // TODO: Render 3D to FBO texture and display here
    
    ImGui::End();
}

// Controls Panel
if (showControls) {
    UIPanel::render(params, &design, &camera);
}

// Statistics Panel
if (showStats) {
    ImGui::Begin("Statistics", &showStats);
    // ... stats code ...
    ImGui::End();
}
```

#### Step 5: Multi-Viewport Support
```cpp
// After ImGui::Render()
ImGui::Render();
ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

// Update platform windows for multi-monitor
if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow* backup = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup);
}
```

### Option 2: Simple Multi-Window (No Docking)

**Current Implementation (What you have now)**:
- Main window with UI panels
- Works with standard ImGui
- No drag-and-drop docking
- Still allows separate windows

**Future Enhancement**:
Create separate ImGui windows without docking:

```cpp
// 2D View Window (moveable, resizable, but not dockable)
ImGui::Begin("2D Paint View", &show2DView);
// ... content ...
ImGui::End();

// 3D View Window
ImGui::Begin("3D Preview", &show3DView);
// ... content ...
ImGui::End();

// These windows float independently
// User can position manually
// No drag-and-drop docking
```

---

## Advanced: Independent 3D Rendering

To render 3D view in its own window (not just camera info):

### Step 1: Create Framebuffer Object (FBO)
```cpp
// In Renderer.h
class Renderer {
    GLuint framebuffer3D = 0;
    GLuint texture3D = 0;
    GLuint depthRenderbuffer = 0;
    
    void create3DRenderTarget(int width, int height);
    void render3DToTexture(const Camera& camera, int previewMode);
    GLuint get3DTextureID() const { return texture3D; }
};
```

### Step 2: Implement FBO Creation
```cpp
void Renderer::create3DRenderTarget(int width, int height) {
    // Create framebuffer
    glGenFramebuffers(1, &framebuffer3D);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer3D);
    
    // Create texture
    glGenTextures(1, &texture3D);
    glBindTexture(GL_TEXTURE_2D, texture3D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture3D, 0);
    
    // Create depth buffer
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    
    // Check completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
```

### Step 3: Render to Texture
```cpp
void Renderer::render3DToTexture(const Camera& camera, int previewMode) {
    // Bind FBO
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer3D);
    glViewport(0, 0, width, height);
    
    // Clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render 3D scene
    // ... (your existing 3D render code) ...
    
    // Unbind FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
```

### Step 4: Display in ImGui Window
```cpp
if (show3DView) {
    ImGui::Begin("3D Preview", &show3DView);
    
    ImVec2 viewSize = ImGui::GetContentRegionAvail();
    
    // Resize FBO if window size changed
    static ImVec2 lastSize = ImVec2(0, 0);
    if (viewSize.x != lastSize.x || viewSize.y != lastSize.y) {
        renderer.create3DRenderTarget((int)viewSize.x, (int)viewSize.y);
        lastSize = viewSize;
    }
    
    // Render 3D to texture
    renderer.render3DToTexture(camera, params.previewMode);
    
    // Display texture
    ImGui::Image((void*)(intptr_t)renderer.get3DTextureID(), viewSize,
                ImVec2(0, 1), ImVec2(1, 0));
    
    ImGui::End();
}
```

---

## Benefits of Full Docking Implementation

### Productivity
- ✅ Paint in 2D, see instant 3D feedback
- ✅ Drag windows to second monitor
- ✅ Save custom layouts
- ✅ Tab groups (stack multiple windows)

### Workflow
- ✅ **Paint Focus**: 2D view large, 3D small preview
- ✅ **3D Focus**: 3D large, 2D thumbnail
- ✅ **Analysis Mode**: Stats + biome view + 3D
- ✅ **Export Ready**: All panels visible for screenshots

### Professional Feel
- ✅ Unity/Unreal/Blender-style interface
- ✅ Multi-monitor support
- ✅ Flexible workspace
- ✅ User-customizable

---

## Migration Checklist

When ready to implement docking:

- [ ] Install ImGui docking branch
- [ ] Enable docking flags in ImGui init
- [ ] Create fullscreen dockspace
- [ ] Convert UI panels to dockable windows
- [ ] Implement FBO for independent 3D rendering
- [ ] Add menu bar with window toggles
- [ ] Implement layout save/load
- [ ] Add layout presets
- [ ] Test multi-monitor support
- [ ] Update documentation

---

## Estimated Implementation Time

- **Docking Setup**: 1 hour
- **Window Refactoring**: 2 hours
- **FBO 3D Rendering**: 3 hours
- **Layout System**: 2 hours
- **Testing & Polish**: 2 hours

**Total**: ~10 hours for full professional docking system

---

## Recommended Approach

### Phase 1 (Now):
- ✅ Use current single-window system
- ✅ Keep bug fixes and engine presets
- ✅ Test and refine features

### Phase 2 (When Ready):
- Upgrade to ImGui docking branch
- Implement basic dockable windows
- Add 2D + 3D separate windows

### Phase 3 (Polish):
- Add FBO for true independent 3D rendering
- Implement layout save/load
- Add presets and themes

---

*This guide will help you implement dockable windows when you're ready to upgrade ImGui!*
