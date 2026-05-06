#include "viewer.h"


void Viewer::run()
{
    auto mesh = OBJLoader::load(obj_path, filename);
    Mesh_AABB mesh_aabb = getMeshAABB(mesh);
    Camera camera(mesh_aabb.center, mesh_aabb.radius * 3.0f);
    Texture texture(obj_path, filename);
    Light light;
    Rasterizer rasterizer(height, height);

    rasterizer_width = height;
    rasterizer_height = height;

    bool dragging = false;
    bool right_dragging = false;

    while (running)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            // 将SDL事件交给ImGui
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_KEY_DOWN)
                if (event.key.key == SDLK_ESCAPE)
                    running = false;

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                    dragging = true;
                if (event.button.button == SDL_BUTTON_RIGHT)
                    right_dragging = true;
            }
                
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                    dragging = false;
                if (event.button.button == SDL_BUTTON_RIGHT)
                    right_dragging = false;
            }

            if (event.type == SDL_EVENT_MOUSE_MOTION)
            {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;

                bool in_viewer = mouse_x >= 0 && mouse_x < height && mouse_y >= 0 && mouse_y < height;

                if (right_dragging && in_viewer)
                {
                    float dx = event.motion.xrel;
                    float dy = event.motion.yrel;

                    camera.model_yaw += dx * 0.3f;
                    camera.model_pitch -= dy * 0.3f;

                    camera.setModelMatrix();
                }

                if (dragging && in_viewer)
                {
                    float dx = event.motion.xrel;
                    float dy = event.motion.yrel;

                    camera.rotate(-dx * 0.3f, dy * 0.3f);
                }
            }
            if (event.type == SDL_EVENT_MOUSE_WHEEL)
                camera.zoom(event.wheel.y * 0.3f);
        }

        // =========================
        // 开始新的ImGui Frame
        // =========================
        ImGui_ImplSDLRenderer3_NewFrame();

        ImGui_ImplSDL3_NewFrame();

        ImGui::NewFrame();

        rasterizer.clear();

        rasterizer.draw(mesh, texture, camera, light);

        updateTexture(rasterizer.frame_buffer);

        //camera.rotate(1, 1); //鼠标左键拖动，参数为float delta_yaw, float delta_pitch
        //camera.zoom(0.1); //鼠标滚轮上为减小距离，下为增加距离，参数是相机到模型的距离

        //light.setAmbIntensity(2.0, 2.0, 2.0); // 三个滑动条设置R，G，B，范围为0-10.0f
        //light.setIntensity(100, 100, 100); // 三个滑动条设置R，G，B，范围为0-100.0f
        //light.setPosition(2.0, 2.0, 2.0); // 手动输入x，y，z坐标

        //light.reset(); //按钮，按下则重置光源设置
        //camera.reset(); //按钮，按下则重置光源设置

        SDL_RenderClear(renderer);

        SDL_FRect viewer_rect = { 0, 0, height, height };

        SDL_RenderTexture(renderer, screen_texture, nullptr, &viewer_rect);

        // 设置窗口位置（右侧200像素区域）
        ImGui::SetNextWindowPos(ImVec2((float)height, 0));

        // 设置窗口大小
        ImGui::SetNextWindowSize(ImVec2((float)(width - height), (float)height));

        // 创建控制面板
        ImGui::Begin("Control Panel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

        // =========================
        // Camera UI
        // =========================
        ImGui::Text("Camera");

        ImGui::Separator();

        ImGui::Text("Yaw: %.2f", camera.yaw);

        ImGui::Text("Pitch: %.2f", camera.pitch);

        ImGui::Text("model_yaw: %.2f", camera.model_yaw);

        ImGui::Text("model_pitch: %.2f", camera.model_pitch);

        ImGui::Text("Distance: %.2f", camera.distance);

        ImGui::Text("Position: (%.2f, %.2f, %.2f)", camera.position.x(), camera.position.y(), camera.position.z());

        if (ImGui::Button("Reset Camera"))
            camera.reset();

        ImGui::Spacing();

        // =========================
        // Light UI
        // =========================
        ImGui::Text("Light");

        ImGui::Separator();

        // 光源位置
        ImGui::SliderFloat3("Position", light.position.data(), -10.0f, 10.0f);

        // 光照强度
        ImGui::SliderFloat3("Intensity", light.intensity.data(), 0.0f, 100.0f);

        // 环境光强度
        ImGui::SliderFloat("Ambient", &light.amb_intensity, 0.0f, 5.0f);

        // 高光范围
        ImGui::SliderFloat("Shininess", &light.shininess, 20.0f, 300.0f);

        if (ImGui::Button("Reset Light"))
            light.reset();

        ImGui::Spacing();

        // =========================
        // Viewer信息
        // =========================
        ImGui::Text("Viewer");

        ImGui::Separator();

        ImGui::Text(
            "Resolution: %d x %d",
            height,
            height
        );

        ImGui::Text(
            "FPS: %.1f",
            ImGui::GetIO().Framerate
        );

        // 结束窗口
        ImGui::End();

        // ==================================================
        // 渲染ImGui
        // ==================================================
        ImGui::Render();

        ImGui_ImplSDLRenderer3_RenderDrawData(
            ImGui::GetDrawData(),
            renderer
        );

        SDL_RenderPresent(renderer);
    }

    rasterizer.save("./output", filename);
}

void Viewer::updateTexture(const std::vector<Eigen::Vector3f>& frame_buffer)
{
    for (int i = 0; i < rasterizer_width * rasterizer_height; i++)
    {
        pixels[i * 3 + 0] = uint8_t(std::clamp(frame_buffer[i].x(), 0.0f, 1.0f) * 255);
        pixels[i * 3 + 1] = uint8_t(std::clamp(frame_buffer[i].y(), 0.0f, 1.0f) * 255);
        pixels[i * 3 + 2] = uint8_t(std::clamp(frame_buffer[i].z(), 0.0f, 1.0f) * 255);
    }

    SDL_UpdateTexture(screen_texture, nullptr, pixels.data(), height * 3);
}


Viewer::Viewer(int height, int width, std::string obj_path, std::string filename): height(height), width(width), obj_path(obj_path), filename(filename)
{

    if (!SDL_Init(SDL_INIT_VIDEO))
        throw std::runtime_error(SDL_GetError());

    window = SDL_CreateWindow("Rasterizer Viewer", width, height, 0);

    if (!window)
        throw std::runtime_error(SDL_GetError());

    renderer = SDL_CreateRenderer(window, nullptr);

    if (!renderer)
        throw std::runtime_error(SDL_GetError());

    screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, height, height);

    if (!screen_texture)
        throw std::runtime_error(SDL_GetError());
    
    pixels.resize(height * height * 3);

    // ==================================================
    // 初始化ImGui
    // ==================================================
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    // 启用键盘导航
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // 使用Dark主题
    ImGui::StyleColorsDark();

    // SDL3 backend
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);

    // SDL_Renderer backend
    ImGui_ImplSDLRenderer3_Init(renderer);
}

Viewer::~Viewer()
{
    // =========================
    // 关闭ImGui
    // =========================
    ImGui_ImplSDLRenderer3_Shutdown();

    ImGui_ImplSDL3_Shutdown();

    ImGui::DestroyContext();

    // =========================
    // SDL资源释放
    // =========================
    SDL_DestroyTexture(screen_texture);

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);

    SDL_Quit();
}