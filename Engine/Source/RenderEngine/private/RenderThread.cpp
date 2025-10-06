#include <RenderCommands.h>
#include <RenderEngine.h>
#include <RenderThread.h>

using namespace GameEngine::Render;

namespace
{
	// Function to run render thread
	static void RunThisThread(void* thisPtr)
	{
		RenderThread* const self = reinterpret_cast<RenderThread*>(thisPtr);
		self->Run();
	}
}

namespace GameEngine::Render
{
	RenderThread::RenderThread()
	{
		assert(m_CurMainFrame == 0);

		s_MainThreadId = std::this_thread::get_id();

		frameMutex[m_CurMainFrame].lock();

		m_Thread = std::make_unique<std::jthread>(RunThisThread, this);
		//m_Thread->detach();
		/// can't control thread if using detach()
	}

	RenderThread::~RenderThread()
	{
		m_Running = false; // stop the update loop
		frameMutex[m_CurMainFrame].unlock(); // first frame locked at initialisation -> last frame unlocked at destruction
		m_Thread->join(); // close thread
		delete m_RenderEngine;
	}

	// Render Loop
	void RenderThread::Run()
	{
		s_RenderThreadId = std::this_thread::get_id();

		m_RenderEngine = new RenderEngine();

		m_RenderEngineIsReady.release();

		while (m_Running)
		{
			std::lock_guard<std::mutex> lock(frameMutex[m_CurrRenderFrame]);

			ProcessCommands();

			m_RenderEngine->Update(m_CurrRenderFrame);

			m_RenderEngine->OnEndFrame();

			OnEndFrame();
		}
	}

	bool RenderThread::IsRenderThread()
	{
		return s_RenderThreadId == std::this_thread::get_id();
	}

	template<typename... Args>
	void RenderThread::EnqueueCommand(ERC command, Args... args)
	{
		switch (command)
		{
		case ERC::CreateRenderObject:
			m_commands[m_CurMainFrame].push_back(
				new EnqueuedRenderCommand(
					[this](RenderCore::Geometry* geometry, RenderObject* renderObject) { m_RenderEngine->CreateRenderObject(geometry, renderObject); },
					std::forward<Args>(args)...)
			);
			break;
		default:
			assert(0);
			break;
		}

		if (IsRenderThread())
		{
			RenderCommand* renderCommand = m_commands[m_CurMainFrame].back();
			renderCommand->DoTask();
			delete renderCommand;
			m_commands[m_CurMainFrame].pop_back();
		}
	}

	void RenderThread::ProcessCommands()
	{
		for (auto& command : m_commands[m_CurrRenderFrame])
		{
			command->DoTask();
		}

		m_commands[m_CurrRenderFrame].clear();
	}

	void RenderThread::OnEndFrame()
	{
		SwitchFrame();
	}

	void RenderThread::SwitchFrame()
	{
		if (IsRenderThread())
		{
			m_CurrRenderFrame = GetNextFrame(m_CurrRenderFrame);
		}
		else
		{
			frameMutex[m_CurMainFrame].unlock();

			m_CurMainFrame = GetNextFrame(m_CurMainFrame);

			frameMutex[m_CurMainFrame].lock();
		}
	}

	size_t RenderThread::GetNextFrame(size_t frameNumber) const
	{
		return (frameNumber + 1) % RenderCore::g_FrameBufferCount;
	}

	RenderEngine* RenderThread::GetRenderEngine()
	{
		return m_RenderEngine;
	}

	void RenderThread::WaitForRenderEngineToInit()
	{
		m_RenderEngineIsReady.acquire();
	}
}