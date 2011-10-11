#ifndef __MU2_CONTROL_H__
#define __MU2_CONTROL_H__

extern int MU2_SetUserID(unsigned short id, unsigned short password);
extern int MU2_SetGroupID(unsigned char id);
extern int MU2_SetEquipmentID(unsigned char id);
extern int MU2_SetDestID(unsigned char id);
extern int MU2_SetRoute(unsigned char route[], int n);
extern int MU2_SetChannel(unsigned char ch);


#endif
