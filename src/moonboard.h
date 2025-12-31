#ifndef MOONBOARD_H
#define MOONBOARD_H

enum moonboard_status_e
{
    MOONBOARD_STATUS_SUCCESS = 0,
    MOONBOARD_STATUS_FAILURE_LED,
};

enum moonboard_status_e moonboard_init(void);
void moonboard_process(void);

#endif // MOONBOARD_H