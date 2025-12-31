
#include "moonboard.h"

int main(void)
{
        moonboard_init();

        while(1)
        {
                moonboard_process();
        }

        return 0;
}
