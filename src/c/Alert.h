#pragma once

//void alert_window_create(char *Alert);
void alert_create(char *Alert, int Duration, int Type);

void alert_distroy();

Window *alert_get_window();