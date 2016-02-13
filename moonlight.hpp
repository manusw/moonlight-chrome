#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/mouse_lock.h"

#include "ppapi/c/ppb_gamepad.h"

#include "ppapi/utility/completion_callback_factory.h"

#include "nacl_io/nacl_io.h"

#include <Limelight.h>

class MoonlightInstance : public pp::Instance, public pp::MouseLock {
    public:
        explicit MoonlightInstance(PP_Instance instance) :
            pp::Instance(instance),
            pp::MouseLock(this),
            m_CallbackFactory(this),
            m_MouseLocked(false) {            
            // This function MUST be used otherwise sockets don't work (nacl_io_init() doesn't work!)            
            nacl_io_init_ppapi(pp_instance(), pp::Module::Get()->get_browser_interface());
            
            m_GamepadApi = static_cast<const PPB_Gamepad*>(pp::Module::Get()->GetBrowserInterface(PPB_GAMEPAD_INTERFACE));
        }
        
        virtual ~MoonlightInstance();
        
        bool Init(uint32_t argc, const char* argn[], const char* argv[]);
        
        void HandleMessage(const pp::Var& var_message);
        
        bool HandleInputEvent(const pp::InputEvent& event);
        
        void PollGamepads();
        
        void DidLockMouse(int32_t result);
        
        void MouseLockLost();
        
        void OnConnectionStopped(uint32_t unused);
        
        void OnConnectionStarted(uint32_t error);
        
        static void* ConnectionThreadFunc(void* context);
        
        static void ClStageStarting(int stage);

        static void ClStageFailed(int stage, long errorCode);

        static void ClConnectionStarted(void);

        static void ClConnectionTerminated(long errorCode);

        static void ClDisplayMessage(char* message);

        static void ClDisplayTransientMessage(char* message);

    private:
        double m_LastPadTimestamps[4];
        const PPB_Gamepad* m_GamepadApi;
        static CONNECTION_LISTENER_CALLBACKS s_ClCallbacks;
        pp::CompletionCallbackFactory<MoonlightInstance> m_CallbackFactory;
        bool m_MouseLocked;
};

extern MoonlightInstance* g_Instance;