#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////
/// Helper functions
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

namespace ImGui {
template <typename F>
bool InputTextLambda(
    const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, F callback = nullptr,
    void* user_data = nullptr) {
    auto freeCallback = [](ImGuiInputTextCallbackData* data) {
        auto& f = *static_cast<F*>(data->UserData);
        return f(data);
    };
    return InputText(label, buf, buf_size, flags, freeCallback, &callback);
}
} // namespace ImGui

int main(void) {
    // Register error callback and then initialize glfw
    glfwSetErrorCallback(error_callback);
    if (glfwInit() != GLFW_TRUE) {
        exit(EXIT_FAILURE);
    }

    // Create a window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Simple example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    // Try to load GLAD
    if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    struct KeyAssignment {
        int sourceKey = 0;
        int destinationKey = 0;
    };
    std::vector<KeyAssignment> keys = { KeyAssignment{ 0, 0 } };
    char inputSourceKey = { '0' }, inputDestinationKey = { '0' };
    size_t activeIndex = 0;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Start frame
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ 300, static_cast<float>(display_h) });
        ImGui::Begin("Key Assignment Controls", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        
        if (ImGui::CollapsingHeader("Controls", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            ImGui::Spacing();
            ImGui::Spacing();
            if (ImGui::Button("Load script", { 115, 0 })) {
            }
            ImGui::SameLine(154);
            if (ImGui::Button("Export script", { 115, 0 })) {
            }            
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Unindent();
        }

         if (ImGui::CollapsingHeader("List", nullptr, ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.15, 0.66, 0.15, 1.0 });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.33, 0.7, 0.33, 1.0 });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.0, 0.5, 0.0, 1.0 });
            if (ImGui::Button("Assign New Key", { 240, 25 })) {
                // ImGui::OpenPopup("Key Chooser");
                keys.emplace_back();
                activeIndex = keys.size() - 1;
            }
            ImGui::PopStyleColor(3);

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Spacing();

             auto index = 0;
             for (auto& element : keys) {
                 ImGui::PushID(&element);
                 ImGui::Spacing();
                 ImGui::Spacing();
                 ImGui::Text("On key: p");
                 ImGui::SameLine(170);
                 const bool pressedEdit = ImGui::Button("Edit", { 100, 0 });
                 ImGui::Text("Do key: w");
                 ImGui::SameLine(220);
                 ImGui::PushStyleColor(ImGuiCol_Button, { 0.66, 0.15, 0.15, 1 });
                 ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.7, 0.33, 0.33, 1 });
                 ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.5, 0.0, 0.0, 1 });
                 const bool pressedDelete = ImGui::Button("Delete", { 50, 0 });
                 ImGui::PopStyleColor(3);
                 ImGui::Spacing();
                 ImGui::Spacing();
                 ImGui::Separator();
                 ImGui::PopID();
                 if (pressedEdit) {

                 } else if (pressedDelete) {
                     keys.erase(keys.begin() + index);
                     break;
                 }
                 ++index;
             }
            ImGui::Unindent();
        }
        
        ImGui::End();

        /*if (ImGui::BeginPopupModal("Key Chooser", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
            ImGui::SetWindowSize({ 300, 150 });
            ImGui::Text("Type the key you want to press:");
            ImGui::InputText("Source", &inputSourceKey, ImGuiInputTextFlags_CallbackCharFilter, 1);
            ImGui::Text("Type the key you want it to map to:");
            ImGui::InputText("Target", &inputDestinationKey, ImGuiInputTextFlags_CallbackCharFilter, 1);
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            if (ImGui::Button("Close", { 100, 20 })) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine(240);
            if (ImGui::Button("Accept", { 50, 20 })) {
                keys.push_back({ static_cast<int>(inputSourceKey), static_cast<int>(inputDestinationKey) });
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::End();*/

        // Rendering
        ImGui::Render();
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
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
    exit(EXIT_SUCCESS);
}

static void error_callback(int /*error*/, const char* description) { fprintf(stderr, "Error: %s\n", description); }
static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}