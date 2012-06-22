#include <rtgui/dc.h>
#include <rtgui/dc_hw.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/widgets/container.h>
#include "demo_view.h"

#define RAND(x1, x2) ((rand() % (x2 - x1)) + x1)

static struct rtgui_container *container = RT_NULL;
static int running = 0;

void _onidle(struct rtgui_object *object, rtgui_event_t *event)
{
	rtgui_color_t color;
	rtgui_rect_t rect, draw_rect;
	struct rtgui_dc *dc;

	/* ��ÿؼ�������DC */
	// dc = rtgui_dc_hw_create(RTGUI_WIDGET(container)); 
	dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(container));
	if (dc == RT_NULL)
		return;

	demo_view_get_logic_rect(RTGUI_CONTAINER(container), &rect);
	draw_rect.x1 = RAND(rect.x1, rect.x2);
	draw_rect.y1 = RAND(rect.y1, rect.y2);
	draw_rect.x2 = RAND(draw_rect.x1, rect.x2);
	draw_rect.y2 = RAND(draw_rect.y1, rect.y2);

	color = RTGUI_RGB(rand() % 255, rand() % 255, rand() % 255);
	RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET(container)) = color;

	rtgui_dc_fill_rect(dc, &draw_rect);

	/* ��ͼ��� */
	rtgui_dc_end_drawing(dc);
}

void _draw_default(struct rtgui_object *object, rtgui_event_t* event)
{
	struct rtgui_widget *widget = RTGUI_WIDGET(object);
	struct rtgui_dc* dc;
	rtgui_rect_t rect;

	/* ��Ϊ�õ���demo container�����汾����һ���ֿؼ��������ڻ�ͼʱ��Ҫ��demo container�Ȼ�ͼ */
	rtgui_container_event_handler(object, event);

	/* ��ÿؼ�������DC */
	dc = rtgui_dc_begin_drawing(widget);
	if (dc == RT_NULL) /* ��������������DC�����أ�����ؼ��򸸿ؼ�������״̬��DC�ǻ�ȡ���ɹ��ģ� */
		return;

	/* ���demo container������ͼ������ */
	demo_view_get_logic_rect(RTGUI_CONTAINER(widget), &rect);

	/* �������� */
	RTGUI_WIDGET_BACKGROUND(widget) = default_background;
	rtgui_dc_fill_rect(dc, &rect);

	/* ��ʾ��ʾ */
	rtgui_dc_draw_text(dc, "���������ʼ/ֹͣ����...", &rect);

	/* ��ͼ��� */
	rtgui_dc_end_drawing(dc);
}

rt_bool_t benchmark_event_handler(struct rtgui_object *object, rtgui_event_t *event)
{
	if (event->type == RTGUI_EVENT_PAINT)
	{
		_draw_default(object, event);
	}
	else if (event->type == RTGUI_EVENT_KBD)
	{
		struct rtgui_event_kbd *kbd = (struct rtgui_event_kbd*)event;

		if (RTGUI_KBD_IS_UP(kbd))
		{
			if (running)
			{
				/* stop */
				rtgui_application_set_onidle(RT_NULL);
				_draw_default(object, event);
			}
			else
			{
				/* run */
				rtgui_application_set_onidle(_onidle);
			}

			running = !running;
		}
		return RT_TRUE;
	}
	else
	{
		/* ����Ĭ�ϵ��¼��������� */
		return rtgui_container_event_handler(object, event);
	}

	return RT_FALSE;
}

rtgui_container_t *demo_view_benchmark(void)
{
	srand(100);
	container = demo_view("��ͼ����");
	rtgui_object_set_event_handler(RTGUI_OBJECT(container), benchmark_event_handler);

	return container;
}