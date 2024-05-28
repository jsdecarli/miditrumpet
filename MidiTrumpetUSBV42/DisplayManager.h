#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

extern void init_display();

extern void display_note(int iNote, int iSlider, int iValveIndex, int iValveMap, byte & iOldDisplayNote);

extern void clear_display();
#endif