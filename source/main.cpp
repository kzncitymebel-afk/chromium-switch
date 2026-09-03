#include <switch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imgui.h>
#include <imgui_impl_deko3d.h>
#include <deko3d.h>

static DkDevice device;
static DkQueue queue;
static DkCmdBuf cmdBuf;
static DkSwapchain swapchain;
static int fbWidth, fbHeight;

void initGraphics() {
    dkDeviceCreate(&device, NULL);

    DkQueueConfig queueConfig;
    dkQueueConfigDefault(&queueConfig);
    queueConfig.device = device;
    dkQueueCreate(&queue, &queueConfig);

    DkCmdBufConfig cmdBufConfig;
    dkCmdBufConfigDefault(&cmdBufConfig);
    cmdBufConfig.device = device;
    dkCmdBufCreate(&cmdBuf, &cmdBufConfig);

    framebufferGetResolution(&fbWidth, &fbHeight);
    
    DkSwapchainConfig swapchainConfig;
    dkSwapchainConfigDefault(&swapchainConfig);
    swapchainConfig.device = device;
    swapchainConfig.format = DkImageFormat_RGBA8_Unorm;
    swapchainConfig.width = fbWidth;
    swapchainConfig.height = fbHeight;
    swapchainConfig.vblankMode = DkVblankMode_Blocking;
    dkSwapchainCreate(&swapchain, &swapchainConfig);
}

void renderFrame() {
    dkCmdBufBegin(cmdBuf);

    DkImage* frame = dkSwapchainAcquireImage(swapchain);
    if (frame) {
        DkColor clearColor = dkColorMake(0.2f, 0.2f, 0.2f, 1.0f);
        dkCmdBufClearColor(cmdBuf, frame, &clearColor);

        dkCmdBufEnd(cmdBuf);
        dkQueueSubmitCommands(queue, cmdBuf);
        dkQueuePresent(queue, &swapchain, 1);
    }
}

int main(int argc, char* argv[]) {
    consoleInit(NULL);
    initGraphics();
    
    ImGui::CreateContext();
    ImGui_ImplDeko3D_Init(device, queue, fbWidth, fbHeight);
    
    printf("Chromium Port Starting...\n");
    printf("Base: deko3d + ImGui\n");
    
    bool running = true;
    while (appletMainLoop() && running) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        
        if (kDown & KEY_PLUS) {
            running = false;
            break;
        }
        
        ImGui_ImplDeko3D_NewFrame();
        ImGui::NewFrame();
        
        ImGui::Begin("Chromium Port - Switch");
        ImGui::Text("Welcome! This is the base.");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        if (ImGui::Button("Exit (or press +)")) {
            running = false;
        }
        ImGui::End();
        
        ImGui::Render();
        ImGui_ImplDeko3D_RenderDrawData(ImGui::GetDrawData());
        renderFrame();
    }
    
    ImGui_ImplDeko3D_Shutdown();
    ImGui::DestroyContext();
    consoleExit(NULL);
    
    return 0;
}
