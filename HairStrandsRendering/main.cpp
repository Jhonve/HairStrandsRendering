#include "Viewer.h"
#include "elems/camera.h"

class Application
{
public:
    Application(const std::string& app_name)
    {
        m_window = std::make_unique<GLWindow>();
        m_window->init(1024, 720, app_name);
    }
  
    static Application& Instance() { return *m_instance; }
  
    void loop()
    {
        while (m_window->is_running())
        {
          m_window->render();
        }
    }

private:
    static Application* m_instance;

    std::unique_ptr<GLWindow> m_window;
};

int main()
{
    auto app = std::make_unique<Application>("StrandsViewe");
    app->loop();

    return 0;
}