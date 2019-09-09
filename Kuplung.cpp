#include "Kuplung.hpp"

Kuplung::Kuplung() {
  this->g_Allocator = NULL;
  this->g_Instance = VK_NULL_HANDLE;
  this->g_PhysicalDevice = VK_NULL_HANDLE;
  this->g_Device = VK_NULL_HANDLE;
  this->g_QueueFamily = (uint32_t) - 1;
  this->g_Queue = VK_NULL_HANDLE;
  this->g_DebugReport = VK_NULL_HANDLE;
  this->g_PipelineCache = VK_NULL_HANDLE;
  this->g_DescriptorPool = VK_NULL_HANDLE;

  this->g_MinImageCount = 2;
  this->g_SwapChainRebuild = false;
  this->g_SwapChainResizeWidth = 0;
  this->g_SwapChainResizeHeight = 0;

  this->managerUI = std::make_unique<UIManager>();
}

Kuplung::~Kuplung() {
  this->managerUI.reset();

	vkDeviceWaitIdle(this->g_Device);
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	this->CleanupVulkanWindow();
	this->CleanupVulkan();

  SDL_DestroyWindow(this->sdlWindow);
  this->sdlWindow = NULL;

  SDL_Quit();
}

int Kuplung::run() {
  if (!this->init())
    return 1;

  SDL_Event ev;
  int frameCounter = 1;

  // Our state
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(Settings::Instance()->guiClearColor.r, Settings::Instance()->guiClearColor.g, Settings::Instance()->guiClearColor.b, Settings::Instance()->guiClearColor.w);

  while (this->gameIsRunning) {
    float fts = (1.0 * std::clock() / CLOCKS_PER_SEC);

    while (SDL_PollEvent(&ev)) {
      this->onEvent(&ev);
      ImGui_ImplSDL2_ProcessEvent(&ev);
    }

    // rendering
    this->managerUI->renderStart();
    this->renderScene();

    if (this->gameIsRunning == false)
      break;

    if (this->g_SwapChainRebuild) {
      this->g_SwapChainRebuild = false;
      ImGui_ImplVulkan_SetMinImageCount(this->g_MinImageCount);
      ImGui_ImplVulkanH_CreateWindow(
        this->g_Instance, this->g_PhysicalDevice, this->g_Device,
        &this->g_MainWindowData, this->g_QueueFamily, this->g_Allocator,
        this->g_SwapChainResizeWidth, this->g_SwapChainResizeHeight, this->g_MinImageCount);
      this->g_MainWindowData.FrameIndex = 0;
    }

    // Start the Dear ImGui frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(this->sdlWindow);
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
      float f = 0.0f;
      int counter = 0;

      ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

      ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
      ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
      ImGui::Checkbox("Another Window", &show_another_window);

      ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::ColorEdit3("clear color", (float*)& clear_color); // Edit 3 floats representing a color

      if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window) {
      ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me"))
        show_another_window = false;
      ImGui::End();
    }

    // Rendering
    ImGui::Render();
    memcpy(&this->windowVulkan->ClearValue.color.float32[0], &clear_color, 4 * sizeof(float));
    this->FrameRender(this->windowVulkan);
    this->FramePresent(this->windowVulkan);

    this->managerUI->renderEnd();

    if (Settings::Instance()->showFrameRenderTime) {
      if (frameCounter > ImGui::GetIO().Framerate) {
        float fte = (1.0 * std::clock() / CLOCKS_PER_SEC);
        Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[TIMINGS] FRAME draw time : %f ms (%f seconds)", (fte - fts) * 1000, (fte - fts)));
        // Settings::Instance()->logTimings(__FILE__, __func__);
        frameCounter = 1;
      }
      else
        frameCounter += 1;
    }
  }

  return 0;
}

bool Kuplung::init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("[KuplungVK] Error: SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    return false;
  }
  this->doLog("[INIT] SDL initialized.");

  this->initFolders();
  this->doLog("[INIT] Folders initialized.");

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(Settings::Instance()->SDL_Window_Flags);
	sdlWindow = SDL_CreateWindow(this->WINDOW_TITLE, this->WINDOW_POSITION_X, this->WINDOW_POSITION_Y, Settings::Instance()->SDL_Window_Width, Settings::Instance()->SDL_Window_Height, window_flags);
  this->doLog("[INIT] SWL Window created.");

  Settings::Instance()->setLogFunc(std::bind(&Kuplung::doLog, this, std::placeholders::_1));
  this->doLog("[INIT] Log function set.");

  this->managerControls = std::make_unique<ControlsManager>();
  this->managerControls->init(this->sdlWindow);
  this->doLog("[INIT] Input Control Manager initialized.");

  this->gameIsRunning = true;

	// Setup Vulkan
	uint32_t extensions_count = 0;
	SDL_Vulkan_GetInstanceExtensions(this->sdlWindow, &extensions_count, NULL);
	const char** extensions = new const char* [extensions_count];
	SDL_Vulkan_GetInstanceExtensions(this->sdlWindow, &extensions_count, extensions);
	this->SetupVulkan(extensions, extensions_count);
	delete[] extensions;

	// Create Window Surface
	VkSurfaceKHR surface;
	VkResult err;
	if (SDL_Vulkan_CreateSurface(this->sdlWindow, this->g_Instance, &surface) == 0) {
    doLog(Settings::Instance()->string_format("[Init] Error: Failed to create Vulkan surface! SDL Error: %s\n", SDL_GetError()));
    return false;
  }

	// Create Framebuffers
	int w, h;
	SDL_GetWindowSize(this->sdlWindow, &w, &h);
	this->windowVulkan = &this->g_MainWindowData;
	this->SetupVulkanWindow(this->windowVulkan, surface, w, h);

	/*ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = this->g_Instance;
	init_info.PhysicalDevice = this->g_PhysicalDevice;
	init_info.Device = this->g_Device;
	init_info.QueueFamily = this->g_QueueFamily;
	init_info.Queue = this->g_Queue;
	init_info.PipelineCache = this->g_PipelineCache;
	init_info.DescriptorPool = this->g_DescriptorPool;
	init_info.Allocator = this->g_Allocator;
	init_info.MinImageCount = this->g_MinImageCount;
	init_info.ImageCount = this->windowVulkan->ImageCount;
	init_info.CheckVkResultFn = this->checkVKRresult;
  this->managerUI = std::make_unique<this->managerUI>();
  this->managerUI->init(sdlWindow, init_info, this->windowVulkan);*/

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForVulkan(this->sdlWindow);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = this->g_Instance;
	init_info.PhysicalDevice = this->g_PhysicalDevice;
	init_info.Device = this->g_Device;
	init_info.QueueFamily = this->g_QueueFamily;
	init_info.Queue = this->g_Queue;
	init_info.PipelineCache = this->g_PipelineCache;
	init_info.DescriptorPool = this->g_DescriptorPool;
	init_info.Allocator = this->g_Allocator;
	init_info.MinImageCount = this->g_MinImageCount;
	init_info.ImageCount = this->windowVulkan->ImageCount;
	init_info.CheckVkResultFn = KVK_checkVKRresult;
	ImGui_ImplVulkan_Init(&init_info, this->windowVulkan->RenderPass);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Upload Fonts
	{
		// Use any command queue
		VkCommandPool command_pool = this->windowVulkan->Frames[this->windowVulkan->FrameIndex].CommandPool;
		VkCommandBuffer command_buffer = this->windowVulkan->Frames[this->windowVulkan->FrameIndex].CommandBuffer;

		err = vkResetCommandPool(this->g_Device, command_pool, 0);
		KVK_checkVKRresult(err);
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(command_buffer, &begin_info);
		KVK_checkVKRresult(err);

		ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

		VkSubmitInfo end_info = {};
		end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		end_info.commandBufferCount = 1;
		end_info.pCommandBuffers = &command_buffer;
		err = vkEndCommandBuffer(command_buffer);
		KVK_checkVKRresult(err);
		err = vkQueueSubmit(this->g_Queue, 1, &end_info, VK_NULL_HANDLE);
		KVK_checkVKRresult(err);

		err = vkDeviceWaitIdle(this->g_Device);
		KVK_checkVKRresult(err);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

  return true;
}

void Kuplung::renderScene() {
}

void Kuplung::onEvent(SDL_Event* ev) {
  this->managerUI->processEvent(ev);
  this->managerControls->processEvents(ev);

  this->gameIsRunning = this->managerControls->gameIsRunning;

  // window resize
  if (ev->type == SDL_WINDOWEVENT) {
    switch (ev->window.event) {
      case SDL_WINDOWEVENT_SIZE_CHANGED:
      case SDL_WINDOWEVENT_RESIZED:
        Settings::Instance()->SDL_Window_Width = static_cast<int>(ev->window.data1);
        Settings::Instance()->SDL_Window_Height = static_cast<int>(ev->window.data2);
        Settings::Instance()->saveSettings();
        if (ev->window.windowID == SDL_GetWindowID(this->sdlWindow)) {
          this->g_SwapChainResizeWidth = (int)ev->window.data1;
          this->g_SwapChainResizeHeight = (int)ev->window.data2;
          this->g_SwapChainRebuild = true;
        }
        break;
    }
  }
}

void Kuplung::doLog(const std::string& message) {
#ifdef _WIN32
  OutputDebugString(CPPUtilities::convert_to_wstring("[KuplungVK] " + message + "\n").c_str());
#else
  printf("[KuplungVK] %s\n", message.c_str());
#endif
  //if (this->managerUI)
  //	this->managerUI->doLog("[KuplungVK] " + message);
}

void Kuplung::initFolders() {
  std::string homeFolder(""), iniFolder("");
#ifdef _WIN32
  char const* hdrive = getenv("HOMEDRIVE");
  char const* hpath = getenv("HOMEPATH");
  homeFolder = std::string(hdrive) + std::string(hpath);

  TCHAR szPath[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, szPath))) {
    std::string folderLocalAppData("");
#  ifndef UNICODE
    folderLocalAppData = szPath;
#  else
    std::wstring folderLocalAppDataW = szPath;
    folderLocalAppData = std::string(folderLocalAppDataW.begin(), folderLocalAppDataW.end());
#  endif
    //std::string folderLocalAppData(szPath);
    folderLocalAppData += "\\supudo.net";
    if (!boost::filesystem::exists(folderLocalAppData))
      boost::filesystem::create_directory(folderLocalAppData);
    folderLocalAppData += "\\KuplungVK";
    if (!boost::filesystem::exists(folderLocalAppData))
      boost::filesystem::create_directory(folderLocalAppData);

    std::string current_folder = boost::filesystem::current_path().string() + "\\resources";
    std::string fName("");

    fName = "KuplungVK_Settings.ini";
    std::string iniFileSource(current_folder + "\\" + fName);
    std::string iniFileDestination = folderLocalAppData + "\\" + fName;
    if (!boost::filesystem::exists(iniFileDestination))
      boost::filesystem::copy(iniFileSource, iniFileDestination);

    fName = "KuplungVK_RecentFiles.ini";
    std::string iniFileRecentSource(current_folder + "\\" + fName);
    std::string iniFileRecentDestination = folderLocalAppData + "\\" + fName;
    if (!boost::filesystem::exists(iniFileRecentDestination))
      boost::filesystem::copy(iniFileRecentSource, iniFileRecentDestination);

    fName = "KuplungVK_RecentFilesImported.ini";
    std::string iniFileRecentImportedSource(current_folder + "\\" + fName);
    std::string iniFileRecentImportedDestination = folderLocalAppData + "\\" + fName;
    if (!boost::filesystem::exists(iniFileRecentImportedDestination))
      boost::filesystem::copy(iniFileRecentImportedSource, iniFileRecentImportedDestination);

    iniFolder = folderLocalAppData;
  }
  else
    iniFolder = homeFolder;
#elif defined macintosh // OS 9
  char const* hpath = getenv("HOME");
  homeFolder = std::string(hpath);
  iniFolder = homeFolder;
#else
  char const* hpath = getenv("HOME");
  if (hpath != NULL)
    homeFolder = std::string(hpath);
  iniFolder = homeFolder;
  CFURLRef appUrlRef;
  appUrlRef = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("KuplungVK_Settings.ini"), NULL, NULL);
  CFStringRef filePathRef = CFURLCopyPath(appUrlRef);
  const char* filePath = CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8);
  iniFolder = std::string(filePath);
  homeFolder = std::string(filePath);
  boost::replace_all(iniFolder, "KuplungVK_Settings.ini", "");
  boost::replace_all(homeFolder, "KuplungVK_Settings.ini", "");
  CFRelease(filePathRef);
  CFRelease(appUrlRef);
#endif
  if (Settings::Instance()->ApplicationConfigurationFolder.empty())
    Settings::Instance()->ApplicationConfigurationFolder = iniFolder;
  if (Settings::Instance()->currentFolder.empty())
    Settings::Instance()->currentFolder = homeFolder;
  Settings::Instance()->initSettings(iniFolder);
}

#ifdef IMGUI_VULKAN_DEBUG_REPORT
VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
	(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
	fprintf(stderr, "[KuplungVK] ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
	return VK_FALSE;
}
#endif // IMGUI_VULKAN_DEBUG_REPORT

void Kuplung::selectBestGPU() {
  VkResult err;

  uint32_t gpu_count;
  err = vkEnumeratePhysicalDevices(this->g_Instance, &gpu_count, NULL);
  KVK_checkVKRresult(err);
  IM_ASSERT(gpu_count > 0);

  VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
  err = vkEnumeratePhysicalDevices(this->g_Instance, &gpu_count, gpus);
  KVK_checkVKRresult(err);

  VkPhysicalDeviceProperties device_props;
  vkGetPhysicalDeviceProperties(gpus[0], &device_props);
  printf("GPU 0:\n apiVersion : %i\n driverVersion : %i\n vendorID = %i\n deviceID = %i\n deviceType = %i (2 = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)\n deviceName = %s\n",
    device_props.apiVersion,
    device_props.driverVersion,
    device_props.vendorID,
    device_props.deviceID,
    device_props.deviceType,
    device_props.deviceName
    // device_props.pipelineCacheUUID,
    // device_props.limits,
    // device_props.sparseProperties
  );

  int32_t ddsel = -1;
  VkPhysicalDevice desired_device;
  std::vector<VkPhysicalDevice> gpu_devices = std::vector<VkPhysicalDevice>(gpu_count);
  err = vkEnumeratePhysicalDevices(this->g_Instance, &gpu_count, gpu_devices.data());
  for (const VkPhysicalDevice& device : gpu_devices) {
    auto props = VkPhysicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(device, &props);
    if (props.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      desired_device = device;
      ddsel += 1;
    }
    else if (props.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
      if (ddsel == -1) {
        desired_device = device;
        ddsel += 1;
      }
    }
    else
      doLog("No suitable GPU found!");

    std::string deviceName = props.deviceName;

    // Determine the available device local memory.
    auto memoryProps = VkPhysicalDeviceMemoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(device, &memoryProps);
    auto heapsPointer = memoryProps.memoryHeaps;
    auto heaps = std::vector<VkMemoryHeap>(heapsPointer, heapsPointer + memoryProps.memoryHeapCount);
    for (const auto& heap : heaps) {
      if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
          // Device local heap, should be size of total GPU VRAM.
          //heap.size will be the size of VRAM in bytes. (bigger is better)
          doLog(Settings::Instance()->string_format("Device '%s' with memory : %s", deviceName.c_str(), Settings::Instance()->prettyBbytes(heap.size).c_str()));
      }
    }
  }

  Settings::Instance()->SelectedGPU = ddsel;

  // If a number > 1 of GPUs got reported, you should find the best fit GPU for your purpose
  // e.g. VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU if available, or with the greatest memory available, etc.
  // for sake of simplicity we'll just take the first one, assuming it has a graphics queue family.
  this->g_PhysicalDevice = gpus[Settings::Instance()->SelectedGPU];
  free(gpus);
}

void Kuplung::SetupVulkan(const char** extensions, uint32_t extensions_count) {
	VkResult err;

	// Create Vulkan Instance
	{
    VkApplicationInfo application_info = {
      VK_STRUCTURE_TYPE_APPLICATION_INFO,
      nullptr,
      "KuplungVK",
      VK_MAKE_VERSION(1, 0, 0),
      "Kuplung",
      VK_MAKE_VERSION(1, 0, 0),
      VK_API_VERSION_1_0
    };

		VkInstanceCreateInfo create_info = {
      VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      nullptr, 0,
      &application_info,
      0, nullptr, 0, nullptr
    };
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.enabledExtensionCount = extensions_count;
		create_info.ppEnabledExtensionNames = extensions;

#ifdef IMGUI_VULKAN_DEBUG_REPORT
		// Enabling multiple validation layers grouped as LunarG standard validation
		const char* layers[] = { "VK_LAYER_LUNARG_standard_validation" };
		create_info.enabledLayerCount = 1;
		create_info.ppEnabledLayerNames = layers;

		// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
		const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
		memcpy(extensions_ext, extensions, extensions_count * sizeof(const char*));
		extensions_ext[extensions_count] = "VK_EXT_debug_report";
		create_info.enabledExtensionCount = extensions_count + 1;
		create_info.ppEnabledExtensionNames = extensions_ext;

		// Create Vulkan Instance
		err = vkCreateInstance(&create_info, this->g_Allocator, &this->g_Instance);
		KVK_checkVKRresult(err);
		free(extensions_ext);

		// Get the function pointer (required for any extensions)
		auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(this->g_Instance, "vkCreateDebugReportCallbackEXT");
		IM_ASSERT(vkCreateDebugReportCallbackEXT != NULL);

		// Setup the debug report callback
		VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
		debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_ci.pfnCallback = debug_report;
		debug_report_ci.pUserData = NULL;
		err = vkCreateDebugReportCallbackEXT(this->g_Instance, &debug_report_ci, this->g_Allocator, &this->g_DebugReport);
		KVK_checkVKRresult(err);
#else
		// Create Vulkan Instance without any debug feature
		err = vkCreateInstance(&create_info, this->g_Allocator, &this->g_Instance);
		KVK_checkVKRresult(err);
		IM_UNUSED(this->g_DebugReport);
#endif
	}

  this->selectBestGPU();

	// Select graphics queue family
	{
		uint32_t count;
		vkGetPhysicalDeviceQueueFamilyProperties(this->g_PhysicalDevice, &count, NULL);
		VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
		vkGetPhysicalDeviceQueueFamilyProperties(this->g_PhysicalDevice, &count, queues);
		for (uint32_t i = 0; i < count; i++) {
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				this->g_QueueFamily = i;
				break;
			}
    }
		free(queues);
		IM_ASSERT(this->g_QueueFamily != (uint32_t)-1);
	}

	// Create Logical Device (with 1 queue)
	{
		int device_extension_count = 1;
		const char* device_extensions[] = { "VK_KHR_swapchain" };
		const float queue_priority[] = { 1.0f };
		VkDeviceQueueCreateInfo queue_info[1] = {};
		queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_info[0].queueFamilyIndex = this->g_QueueFamily;
		queue_info[0].queueCount = 1;
		queue_info[0].pQueuePriorities = queue_priority;
		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
		create_info.pQueueCreateInfos = queue_info;
		create_info.enabledExtensionCount = device_extension_count;
		create_info.ppEnabledExtensionNames = device_extensions;
		err = vkCreateDevice(this->g_PhysicalDevice, &create_info, this->g_Allocator, &this->g_Device);
		KVK_checkVKRresult(err);
		vkGetDeviceQueue(this->g_Device, this->g_QueueFamily, 0, &this->g_Queue);
	}

	// Create Descriptor Pool
	{
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		err = vkCreateDescriptorPool(this->g_Device, &pool_info, this->g_Allocator, &this->g_DescriptorPool);
		KVK_checkVKRresult(err);
	}
}

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
void Kuplung::SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height) {
	wd->Surface = surface;

	// Check for WSI support
	VkBool32 res;
	vkGetPhysicalDeviceSurfaceSupportKHR(this->g_PhysicalDevice, this->g_QueueFamily, wd->Surface, &res);
	if (res != VK_TRUE) {
		doLog("[SetupVulkan] Error no WSI support on physical device 0\n");
		exit(-1);
	}

	// Select Surface Format
	const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
	const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
	wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(this->g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

	// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
	VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
	wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(this->g_PhysicalDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
	//printf("[KuplungVK] Selected PresentMode = %d\n", wd->PresentMode);

	// Create SwapChain, RenderPass, Framebuffer, etc.
	IM_ASSERT(this->g_MinImageCount >= 2);
	ImGui_ImplVulkanH_CreateWindow(this->g_Instance, this->g_PhysicalDevice, this->g_Device, wd, this->g_QueueFamily, this->g_Allocator, width, height, this->g_MinImageCount);
}

void Kuplung::CleanupVulkan() {
	vkDestroyDescriptorPool(this->g_Device, this->g_DescriptorPool, this->g_Allocator);

#ifdef IMGUI_VULKAN_DEBUG_REPORT
	// Remove the debug report callback
	auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(this->g_Instance, "vkDestroyDebugReportCallbackEXT");
	vkDestroyDebugReportCallbackEXT(this->g_Instance, this->g_DebugReport, this->g_Allocator);
#endif // IMGUI_VULKAN_DEBUG_REPORT

	vkDestroyDevice(this->g_Device, this->g_Allocator);
	vkDestroyInstance(this->g_Instance, this->g_Allocator);
}

void Kuplung::CleanupVulkanWindow() {
	ImGui_ImplVulkanH_DestroyWindow(this->g_Instance, this->g_Device, &this->g_MainWindowData, this->g_Allocator);
}

void Kuplung::FrameRender(ImGui_ImplVulkanH_Window* wd) {
	VkResult err;

	VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
	err = vkAcquireNextImageKHR(this->g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
	KVK_checkVKRresult(err);

	ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
	{
		err = vkWaitForFences(this->g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
		KVK_checkVKRresult(err);

		err = vkResetFences(this->g_Device, 1, &fd->Fence);
		KVK_checkVKRresult(err);
	}
	{
		err = vkResetCommandPool(this->g_Device, fd->CommandPool, 0);
		KVK_checkVKRresult(err);
		VkCommandBufferBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
		KVK_checkVKRresult(err);
	}
	{
		VkRenderPassBeginInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		info.renderPass = wd->RenderPass;
		info.framebuffer = fd->Framebuffer;
		info.renderArea.extent.width = wd->Width;
		info.renderArea.extent.height = wd->Height;
		info.clearValueCount = 1;
		info.pClearValues = &wd->ClearValue;
		vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
	}

	// Record Imgui Draw Data and draw funcs into command buffer
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), fd->CommandBuffer);

	// Submit command buffer
	vkCmdEndRenderPass(fd->CommandBuffer);
	{
		VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &image_acquired_semaphore;
		info.pWaitDstStageMask = &wait_stage;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &fd->CommandBuffer;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &render_complete_semaphore;

		err = vkEndCommandBuffer(fd->CommandBuffer);
		KVK_checkVKRresult(err);
		err = vkQueueSubmit(this->g_Queue, 1, &info, fd->Fence);
		KVK_checkVKRresult(err);
	}
}

void Kuplung::FramePresent(ImGui_ImplVulkanH_Window* wd) {
	VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
	VkPresentInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &render_complete_semaphore;
	info.swapchainCount = 1;
	info.pSwapchains = &wd->Swapchain;
	info.pImageIndices = &wd->FrameIndex;
	VkResult err = vkQueuePresentKHR(this->g_Queue, &info);
	KVK_checkVKRresult(err);
	wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
}
