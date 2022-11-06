#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <iomanip>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imgui_canvas.h"

#include "shader.h"



// ====================================================================================================================
inline int rangedRand(const int _min, const int _max) {
  int n = _max - _min + 1;
  int remainder = RAND_MAX % n;
  int x;
  do {
    x = rand();
  } while (x >= RAND_MAX - remainder);
  return _min + x % n;
}
// ====================================================================================================================










// ====================================================================================================================
static void glfw_error_callback (int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
// ====================================================================================================================








// ====================================================================================================================
int main (int argc, char** argv) {

  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

  // Decide GL+GLSL versions
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(1920, 1200, "Dear ImGui canvas demo", NULL, NULL);
  if (window == NULL)
    return 1;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    fprintf(stderr, "Failed to initialize GLAD\n");
    return 1;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGui::StyleColorsDark(); // Setup Dear ImGui style

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // ==================================================================================================================
  Shader shader(Shader::ReadFile(BASE_VERTEX_SHADER_FILE_STR), Shader::ReadFile(MASK_FRAGMENT_SHADER_FILE_STR));
  shader.Initialize();
  shader.BindFragDataLocation();
  shader.Use();
  shader.CreateTexture();

  // ------------------------------------------------------------------------------------------------------------------
  int width = 320, height = 240;
  float imageAspectRatio = (float)height/(float)width;
  uint16_t* imageData = new uint16_t[width*height]();
  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      imageData[x + y*width] = (uint16_t)ImPow(2.0,16)*(float)x/(float)width;

  uint8_t* maskData = new uint8_t[width*height]();
  bool maskEnable=true, updateMask=false;
  static float zoomMultiplier=1.0, zoomMinMultiplier=1.0, zoomMaxMultiplier=8.0;
  ImVec2 viewWindowSize, viewSize = ImVec2(640,480);
  // ------------------------------------------------------------------------------------------------------------------
  static std::vector<ImGuiCanvasShape> shapes;

  static GLuint imageTexture, maskTexture, compositeTexture, renderBuffers, frameBuffers;

  shader.SetupTexture(GL_TEXTURE0, &imageTexture, width, height, imageData, GL_RED, 1, GL_TEXTURE_2D, 0, 0, GL_UNSIGNED_SHORT);
  shader.setUniform("image", 0);

  shader.SetupTexture(GL_TEXTURE1, &maskTexture, width, height, maskData);
  shader.setUniform("mask", 1);

  shader.SetupTexture(GL_TEXTURE2, &compositeTexture, width, height, 0, GL_RGBA);
  shader.BindTexture(0);

  shader.SetupRenderBuffer(&renderBuffers, width, height, &frameBuffers, &compositeTexture);

  // ---- add 2 x hlines and 2 x vlines for roi selection -------------------------------------------------------------
  shapes.clear();
  shapes.push_back(ImGuiCanvasShape("vLine##A_V_ImGuiShape", ImVec2(10.0, height/2.0), ImGuiCanvasShapeType::VLine, {0.0f, (float)height}, ImGuiCanvasClip::Out, true));
  shapes.push_back(ImGuiCanvasShape("vLine##B_V_ImGuiShape", ImVec2(width-10.0, height/2.0), ImGuiCanvasShapeType::VLine, {0.0f, (float)height}, ImGuiCanvasClip::In, true));
  shapes.push_back(ImGuiCanvasShape("hLine##A_H_ImGuiShape", ImVec2(width/2.0, 10.0), ImGuiCanvasShapeType::HLine, {0.0f, (float)width}, ImGuiCanvasClip::Out, true));
  shapes.push_back(ImGuiCanvasShape("hLine##B_H_ImGuiShape", ImVec2(width/2.0, height-10.0), ImGuiCanvasShapeType::HLine, {0.0f, (float)width}, ImGuiCanvasClip::In, true));
  // ==================================================================================================================

  // Main loop
  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
 
    // ================================================================================================================
    if (ImGui::Begin("imgui_canvas demo")) {
      ImGui::Separator(); // ==========================================================================================

      // ---- zoom slider ---------------------------------------------------------------------------------------------
      ImGui::AlignTextToFramePadding();
      ImGui::Text("Zoom                ");
      ImGui::SameLine();
      ImGui::SetNextItemWidth(200);
      ImGui::SliderScalar("##zoom_SliderScalar", ImGuiDataType_Float, &zoomMultiplier, &zoomMinMultiplier, &zoomMaxMultiplier, "%.2f X");
      // ---- zoom slider ---------------------------------------------------------------------------------------------

      // ---- mask ----------------------------------------------------------------------------------------------------
      ImGui::AlignTextToFramePadding();
      ImGui::Text("Mask                ");
      ImGui::SameLine();
      if (ImGui::Checkbox("##mask_enable_Checkbox", &maskEnable)) {
        updateMask = true;
        for (ImGuiCanvasShape& shape: shapes)
          shape.setVisible();
      }

      ImGui::SameLine();

      if (!maskEnable) {
        for (std::vector<ImGuiCanvasShape>::iterator shapeIter = shapes.begin()+4; shapeIter != shapes.end(); ++shapeIter) {
          shapeIter->setVisible(false);
        }
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
      }

      if (ImGui::Button("Square##square_Button"))
        shapes.push_back(ImGuiCanvasShape("square##square_ImGuiShape", {(float)rangedRand(shapes[0].m_center.position.x,shapes[1].m_center.position.x), (float)rangedRand(shapes[2].m_center.position.y,shapes[3].m_center.position.y)}, ImGuiCanvasShapeType::Square, {(float)(width/8.0)}, ImGuiCanvasClip::Out, true));

      ImGui::SameLine();
      if (ImGui::Button("Rectangle##rectangle_Button"))
        shapes.push_back(ImGuiCanvasShape("rectangle##rectangle_ImGuiShape", {(float)rangedRand(shapes[0].m_center.position.x,shapes[1].m_center.position.x), (float)rangedRand(shapes[2].m_center.position.y,shapes[3].m_center.position.y)}, ImGuiCanvasShapeType::Rectangle, {(float)(width/8.0), (float)(height/8.0)}, ImGuiCanvasClip::Out, true));

      ImGui::SameLine();
      if (ImGui::Button("Circle##circle_Button"))
        shapes.push_back(ImGuiCanvasShape("circle##circle_ImGuiShape", {(float)rangedRand(shapes[0].m_center.position.x,shapes[1].m_center.position.x), (float)rangedRand(shapes[2].m_center.position.y,shapes[3].m_center.position.y)}, ImGuiCanvasShapeType::Circle, {(float)(width/8.0)}, ImGuiCanvasClip::Out, true));

      ImGui::SameLine();
      if (ImGui::Button("Ellipse##ellipse_Button"))
        shapes.push_back(ImGuiCanvasShape("ellipse##ellipse_ImGuiShape", {(float)rangedRand(shapes[0].m_center.position.x,shapes[1].m_center.position.x), (float)rangedRand(shapes[2].m_center.position.y,shapes[3].m_center.position.y)}, ImGuiCanvasShapeType::Ellipse, {(float)(width/8.0), (float)(height/8.0)}, ImGuiCanvasClip::Out, true));

      if (!maskEnable) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
      }
      // ---- mask ----------------------------------------------------------------------------------------------------

      shader.UpdateTexture(GL_TEXTURE0, &imageTexture, width, height, imageData, GL_RED, 1, GL_TEXTURE_2D, 0, 0, GL_UNSIGNED_SHORT);
      shader.setUniform("image", 0);

      shader.UpdateTexture(GL_TEXTURE1, &maskTexture, width, height, maskData);
      shader.setUniform("mask", 1);

      shader.UpdateTexture(GL_TEXTURE2, &compositeTexture, width, height, 0, GL_RGBA);
      shader.BindTexture(0);

      shader.UpdateRenderBuffer(&renderBuffers, width, height, &frameBuffers, &compositeTexture);

      {
        viewWindowSize.x = ImGui::GetContentRegionAvail().x;
        viewSize.x = viewWindowSize.x - 14;
        viewSize.y = (int)(viewSize.x*imageAspectRatio);
        viewWindowSize.y = viewSize.y + 14;

        viewSize.x = zoomMultiplier*viewSize.x;
        viewSize.y = zoomMultiplier*viewSize.y;

        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6,6));
        ImGui::BeginChild("canvas_child_window", viewWindowSize, true, ImGuiWindowFlags_None | ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::DrawCanvas("canvas", viewSize, ImVec2(width,height), shapes, (ImTextureID)(intptr_t)compositeTexture, maskData, updateMask);
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleVar();
      }

    }
    ImGui::End();
    // ================================================================================================================

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
// ====================================================================================================================rangedRand