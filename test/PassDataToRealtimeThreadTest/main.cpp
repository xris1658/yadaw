#include "concurrent/PassDataToRealtimeThread.hpp"

#include "common/DisableStreamBuffer.hpp"

#include <thread>

int main()
{
    disableStdOutBuffer();
    YADAW::Concurrent::PassDataToRealtimeThread<int> passDataToRealtimeThread(0);
    std::thread realtimeThread(
        [&passDataToRealtimeThread]()
        {
            while(true)
            {
                const auto& data = passDataToRealtimeThread.get();
                // std::printf("%d\n", data);
                if(data == -1)
                {
                    break;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                passDataToRealtimeThread.swapIfNeeded();
            }
        }
    );
    while(true)
    {
        int newData;
        std::scanf("%d", &newData);
        passDataToRealtimeThread.update(newData);
        if(newData == -1)
        {
            break;
        }
    }
    realtimeThread.join();
}