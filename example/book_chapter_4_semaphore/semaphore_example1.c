#include "example.h"
#include "trochili.h"

#if (EVB_EXAMPLE == CH4_SEMAPHORE_EXAMPLE1)

/* �û��̲߳��� */
#define THREAD_LED_STACK_BYTES  (512)
#define THREAD_LED_PRIORITY     (5)
#define THREAD_LED_SLICE        (20)

#define THREAD_CTRL_STACK_BYTES (512)
#define THREAD_CTRL_PRIORITY    (6)
#define THREAD_CTRL_SLICE       (20)

/* �û��߳�ջ���� */
static TBase32 ThreadLedStack[THREAD_LED_STACK_BYTES/4];
static TBase32 ThreadCTRLStack[THREAD_CTRL_STACK_BYTES/4];

/* �û��̶߳��� */
 TThread ThreadLed;
 TThread ThreadCTRL;

/* �û��ź������� */
static TSemaphore LedSemaphore;


/* Led�̵߳������� */
static void ThreadLedEntry(TArgument arg)
{
    TState state;
    TError error;

    while (eTrue)
    {
        /* Led�߳���������ʽ��ȡ�ź������õ������Led */
        state = TclObtainSemaphore(&LedSemaphore, TCLO_IPC_WAIT, 0, &error);
        if (state == eSuccess)
        {
            EvbLedControl(LED1, LED_ON);
        }

        /* Led�߳���������ʽ��ȡ�ź������õ���Ϩ��Led */
        state = TclObtainSemaphore(&LedSemaphore, TCLO_IPC_WAIT, 0, &error);
        if (state == eSuccess)
        {
            EvbLedControl(LED1, LED_OFF);
        }
    }
}


/* CTRL�̵߳������� */
static void ThreadCtrlEntry(TArgument arg)
{
    TState state;
    TError error;
    while (eTrue)
    {
        /* CTRL�߳���ʱ1�� */
        state =  TclDelayThread(0, TCLM_MLS2TICKS(1000), &error);
        TCLM_ASSERT((state == eSuccess), "");
        TCLM_ASSERT((error == TCLE_THREAD_NONE), "");

        /* CTRL�߳��ͷ��ź��� */
        state = TclReleaseSemaphore(&LedSemaphore, 0, 0, &error);
        TCLM_ASSERT((state == eSuccess), "");
        TCLM_ASSERT((error == TCLE_IPC_NONE), "");
    }
}


/* �û�Ӧ����ں��� */
static void AppSetupEntry(void)
{
    TState state;
    TError error;

    /* ��ʼ���ź��� */
    state = TclCreateSemaphore(&LedSemaphore, 0, 1, TCLP_IPC_DUMMY, &error);
    TCLM_ASSERT((state == eSuccess), "");
    TCLM_ASSERT((error == TCLE_IPC_NONE), "");

    /* ��ʼ��Led�豸�����߳� */
    state = TclCreateThread(&ThreadLed,
                          &ThreadLedEntry, (TArgument)0,
                          ThreadLedStack, THREAD_LED_STACK_BYTES,
                          THREAD_LED_PRIORITY, THREAD_LED_SLICE,
                          &error);
    TCLM_ASSERT((state == eSuccess), "");
    TCLM_ASSERT((error == TCLE_THREAD_NONE), "");

    /* ��ʼ��CTRL�߳� */
    state = TclCreateThread(&ThreadCTRL,
                          &ThreadCtrlEntry, (TArgument)0,
                          ThreadCTRLStack, THREAD_CTRL_STACK_BYTES,
                          THREAD_CTRL_PRIORITY, THREAD_CTRL_SLICE,
                          &error);
    TCLM_ASSERT((state == eSuccess), "");
    TCLM_ASSERT((error == TCLE_THREAD_NONE), "");

    /* ����Led�߳� */
    state = TclActivateThread(&ThreadLed, &error);
    TCLM_ASSERT((state == eSuccess), "");
    TCLM_ASSERT((error == TCLE_THREAD_NONE), "");

    /* ����CTRL�߳� */
    state = TclActivateThread(&ThreadCTRL, &error);
    TCLM_ASSERT((state == eSuccess), "");
    TCLM_ASSERT((error == TCLE_THREAD_NONE), "");
}


/* ������BOOT֮������main�����������ṩ */
int main(void)
{
    /* ע������ں˺���,�����ں� */
    TclStartKernel(&AppSetupEntry,
                   &CpuSetupEntry,
                   &EvbSetupEntry,
                   &EvbTraceEntry);
    return 1;
}
#endif
