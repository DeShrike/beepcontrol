#ifndef BEEPCONTROL_H
#define BEEPCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#define  BUFF_SIZE   16

typedef struct
{
	long  data_type;
	unsigned char data_buff[BUFF_SIZE];
} t_data;

#define TYPE_STRING         (int)1
#define TYPE_TWODOUBLES     (int)2
#define TYPE_ARRAY          (int)3
#define TYPE_DOUBLEANDARRAY (int)4
#define TYPE_INTEGER        (int)5

#ifdef __cplusplus
}
#endif

#endif /* BEEPCONTROL_H */
