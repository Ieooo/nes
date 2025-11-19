#ifndef __APU_H__
#define __APU_H__

#include <stdint.h>
#include <stdbool.h>


#define CPU_CLOCK_NTSC 1789773.0 // Hz
#define SAMPLE_RATE 44100       // output sampling rate
#define FRAME_COUNTER_RATE (CPU_CLOCK_NTSC / 240.0) // APU frame counter 通常以 ~240Hz 更新

typedef struct {
    // mem[0x4000-0x4003]/mem[0x4006-0x4007]
    uint16_t m_timer;
    uint8_t  m_duty;
    uint8_t  m_length_counter_load;
    bool     m_length_counter_halt;
    bool     m_volume_envelope_flag;
    uint8_t  m_volume_envelope_period;
    uint8_t  m_sweep_period;
    bool     m_sweep_enabled;
    uint16_t m_sweep_shift_count;
    bool     m_sweep_negate;

    // runtime status
    uint16_t timer;
    uint8_t  duty;
    uint8_t  length_counter_load;
    bool     length_counter_halt;
    bool     volume_envelope_flag;
    uint8_t  volume_envelope_period;
    uint8_t  sweep_period;
    bool     sweep_enabled;
    uint16_t sweep_shift_count;
    bool     sweep_negate;

    uint8_t  duty_step;
    uint8_t  envelope_value;
    uint8_t  length_counter;
} pulse_t;

typedef struct {
    // mem[0x4008-0x400B]
    uint16_t m_timer;
    uint8_t  m_length_counter;
    bool     m_length_enabled;
    uint8_t  m_linear_counter;
    bool     m_linear_reload;
    uint8_t  m_step;

    // runtime status
    uint16_t timer;
    uint8_t  length_counter;
    bool     length_enabled;
    uint8_t  linear_counter;
    bool     linear_reload;
    uint8_t  step;

} triangle_t;

typedef struct {
    // mem[0x400C-0x400F]
    uint16_t m_timer;
    uint8_t m_duty;
    bool m_volume_envelope_flag;
    uint8_t m_volume_envelope_period;
    bool m_sweep_enable;
    uint8_t m_sweep_period;
    uint8_t m_sweep_shift_count;
    bool m_sweep_negate;


    // runtime status
    uint16_t timer;
    uint8_t duty;
    bool volume_envelope_flag;
    uint8_t volume_envelope_period;
    bool sweep_enable;
    uint8_t sweep_period;
    uint8_t sweep_shift_count;
    bool sweep_negate;
} noise_t;

typedef struct {
    uint32_t clock;
    pulse_t pulse[2];
    triangle_t triangle;
    noise_t noise;
    uint8_t mem[128];
} APU;

APU* apu_init();
void api_exit(APU*);
bool apu_step(APU* apu, float **out_sample, uint32_t cpu_cycles);
void apu_write_register(APU*, uint16_t, uint8_t);
uint8_t apu_get_register(APU*, uint16_t);

#endif


