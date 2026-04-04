# 🎯 Configuration Checklist

## Before You Build - Follow This Exact Order

### ☑️ Part 1: Visual Studio Configuration (5 minutes)

#### Task 1.1: Add glad.c to Project
- [ ] Open Solution Explorer in Visual Studio
- [ ] Right-click **Source Files** folder
- [ ] Choose **Add** → **Existing Item...**
- [ ] Navigate to: `D:\quantum\HeightMapGenerator`
- [ ] Select `glad.c`
- [ ] Click **Add**
- [ ] ✓ You should see `glad.c` appear under Source Files

#### Task 1.2: Configure Include Directories
- [ ] Right-click **HeightMapGenerator** project in Solution Explorer
- [ ] Click **Properties**
- [ ] At the top, set **Configuration**: `All Configurations`
- [ ] At the top, set **Platform**: `x64`
- [ ] Navigate to: `Configuration Properties` → `C/C++` → `General`
- [ ] Click on **Additional Include Directories**
- [ ] Click the dropdown arrow → **<Edit...>**
- [ ] Click the folder icon to add a new line
- [ ] Type or paste: `C:\glad\include`
- [ ] Click **OK**
- [ ] ✓ You should see `C:\glad\include` in the list

#### Task 1.3: Add OpenGL Library
- [ ] Still in Properties window
- [ ] Navigate to: `Configuration Properties` → `Linker` → `Input`
- [ ] Click on **Additional Dependencies**
- [ ] Click the dropdown arrow → **<Edit...>**
- [ ] Add a new line: `opengl32.lib`
- [ ] Click **OK**
- [ ] Click **OK** to close Properties
- [ ] ✓ Configuration saved

---

### ☑️ Part 2: vcpkg Dependencies (Wait for completion)

The `setup-local.ps1` script is currently running and installing:

- [ ] vcpkg installation complete
- [ ] glfw3 installed
- [ ] imgui installed
- [ ] vcpkg integrated with Visual Studio

**How to check:**
- Wait for the PowerShell window to show completion message
- You should see "Configuration Complete!" when done

---

### ☑️ Part 3: Verify Files Exist

Check these files are in `D:\quantum\HeightMapGenerator`:

**Source Files:**
- [ ] main.cpp
- [ ] LandscapeDesigner.cpp
- [ ] Renderer.cpp
- [ ] UIPanel.cpp
- [ ] glad.c ← **Important!**

**Header Files:**
- [ ] Image.h
- [ ] LandscapeDesign.h
- [ ] LandscapeDesigner.h
- [ ] LandscapeParameters.h
- [ ] DisplayMode.h
- [ ] Renderer.h
- [ ] UIPanel.h

**Project Files:**
- [ ] HeightMapGenerator.vcxproj
- [ ] HeightMapGenerator.sln

---

### ☑️ Part 4: First Build Attempt

#### Task 4.1: Clean and Rebuild
- [ ] In Visual Studio, menu: **Build** → **Clean Solution**
- [ ] Wait for clean to complete
- [ ] Menu: **Build** → **Rebuild Solution** (or press `Ctrl+Shift+B`)

#### Task 4.2: Check for Errors

**If build succeeds:**
- [ ] ✓ 0 errors shown in Output window
- [ ] ✓ Success message appears
- [ ] → **Skip to Part 5**

**If you see errors, match them here:**

| Error Message | Fix |
|---------------|-----|
| `Cannot open include file: 'glad/glad.h'` | Go back to Task 1.2 - check C:\glad\include path |
| `Cannot open include file: 'GLFW/glfw3.h'` | Wait for vcpkg to finish (Part 2) |
| `Cannot open include file: 'imgui.h'` | Wait for vcpkg to finish (Part 2) |
| `unresolved external symbol gladLoadGL` | Go back to Task 1.1 - add glad.c |
| `cannot open file 'opengl32.lib'` | Go back to Task 1.3 - add OpenGL lib |

---

### ☑️ Part 5: Run the Application

- [ ] Press **F5** (or menu: **Debug** → **Start Debugging**)
- [ ] Wait for application window to appear

**Expected result:**
- [ ] ✓ Window titled "Landscape Designer V0.2" opens
- [ ] ✓ Grayscale heightmap visible on left side
- [ ] ✓ ImGui panel visible on right side with sliders
- [ ] ✓ Can adjust sliders and see changes in real-time

---

### ☑️ Part 6: Test Functionality

Try these to confirm everything works:

- [ ] Move **Water Level** slider → heightmap should update
- [ ] Move **Slope Threshold** slider → statistics should change
- [ ] Change **Noise Amount** → terrain should get noisy
- [ ] Change **Heightmap Frequency** → terrain waves change
- [ ] Change **Resolution** dropdown → map resolution changes
- [ ] Toggle **Auto Update** off/on → regenerate button appears/disappears

---

## 🎉 Success Criteria

All checkboxes marked = **Ready to develop!**

## 🔧 Still Having Issues?

### Debugging Steps:

1. **Verify GLAD structure:**
   ```
   C:\glad\
     ├── include\
     │   ├── glad\
     │   │   └── glad.h
     │   └── KHR\
     │       └── khrplatform.h
     └── src\
         └── glad.c
   ```

2. **Verify vcpkg integration:**
   ```powershell
   C:\vcpkg\vcpkg.exe integrate install
   ```

3. **Check Visual Studio platform:**
   - Make sure you're building for **x64**, not Win32
   - Top toolbar should show "x64" selected

4. **Restart Visual Studio:**
   - Sometimes integration needs a restart

---

## 📞 Quick Reference

**Project**: `D:\quantum\HeightMapGenerator`
**GLAD**: `C:\glad`
**vcpkg**: `C:\vcpkg` (or check where it installed)

**Key paths to add:**
- Include: `C:\glad\include`
- Lib: `opengl32.lib`

**Key file to add:**
- `glad.c` to project

---

**Ready? Start with Part 1, Task 1.1! ✨**
