#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <direct.h>
#include <filesystem>
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////
/// Useful definitions
struct KeyAssignment {
    std::string sourceKey;
    std::string destinationKey;
};

////////////////////////////////////////////////////////////
/// Helper functions
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void to_ahk(const std::string& filename, const std::vector<KeyAssignment>& keys);

////////////////////////////////////////////////////////////
/// Static variables
static enum class ListenState { none, source, destination } listenState;
static int sourceKeyCode = 0;
static int destinationKeyCode = 0;

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

    // Starting state
    listenState = ListenState::none;
    sourceKeyCode = 0;
    destinationKeyCode = 0;
    const ImVec4 clear_color{ 0.45f, 0.55f, 0.60f, 1.00f };
    std::vector<KeyAssignment> keys = { KeyAssignment{} };
    size_t keyEditIndex = 0;

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
                to_ahk("test", keys);
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
                keys.emplace_back();
                keyEditIndex = keys.size() - 1;
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
                ImGui::Text("On key: ");
                ImGui::SameLine();
                ImGui::Text(element.sourceKey.c_str());
                ImGui::SameLine(170);
                const bool pressedEdit = ImGui::Button("Edit", { 100, 0 });
                ImGui::Text("Do key: ");
                ImGui::SameLine();
                ImGui::Text(element.destinationKey.c_str());
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
                    keyEditIndex = index;
                    ImGui::OpenPopup("Edit Key Mapping");
                } else if (pressedDelete) {
                    keys.erase(keys.begin() + index);
                    break;
                }
                ++index;
            }
            ImGui::Unindent();
        }

        if (ImGui::BeginPopupModal("Edit Key Mapping", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
            ImGui::PushID(keyEditIndex);
            ImGui::SetWindowSize({ 300, 150 });

            ImGui::Text("Type the key you want to press:");
            static char bufferS[32] = { '\0' };
            ImGui::PushID(&bufferS);
            ImGui::InputTextLambda(
                "Source", bufferS, 32, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CallbackAlways,
                [&](ImGuiInputTextCallbackData* data) {
                    listenState = ListenState::source;
                    return 0;
                });
            ImGui::PopID();

            ImGui::Text("Now type the key you want it to map to:");
            static char bufferT[32] = { '\0' };
            ImGui::PushID(&bufferT);
            ImGui::InputTextLambda(
                "Target", bufferT, 32, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_CallbackAlways,
                [&](ImGuiInputTextCallbackData* data) {
                    listenState = ListenState::destination;
                    return 0;
                });
            ImGui::PopID();

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
                keys[keyEditIndex].sourceKey = std::string(&bufferS[0]);
                keys[keyEditIndex].destinationKey = std::string(&bufferT[0]);
                ImGui::CloseCurrentPopup();
            }

            ImGui::PopID();
            ImGui::EndPopup();
        }

        ImGui::End();

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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int /*mods*/) {
    switch (listenState) {
    case ListenState::source:
        sourceKeyCode = scancode;
        break;
    case ListenState::destination:
        destinationKeyCode = scancode;
        break;
    case ListenState::none:
    default:
        break;
    }
    listenState = ListenState::none;
}

static std::string get_current_dir() {
    // Technique to return the running directory of the application
    char cCurrentPath[FILENAME_MAX];
    if (_getcwd(cCurrentPath, sizeof(cCurrentPath)) != nullptr)
        cCurrentPath[sizeof(cCurrentPath) - 1ULL] = '\0';
    return std::string(cCurrentPath);
}

static void to_ahk(const std::string& filename, const std::vector<KeyAssignment>& keys) {
    const std::string finalPath = get_current_dir() + "//" + filename + ".ahk";
    std::ofstream file(finalPath);

    for (const auto& element : keys) {
        file << '`' + element.sourceKey + "::" + '`' + element.destinationKey + "\n";
        file << '`' + element.destinationKey + "::" + '`' + element.sourceKey + "\n\r";
    }
    file << "return\n\r";
    file.close();
}