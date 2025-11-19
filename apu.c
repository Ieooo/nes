#include "apu.h"
#include <stdio.h>
#include <math.h>

static const uint8_t duty_table[4][8] = {
    {0,0,0,0,0,0,0,1},  // 12.5%
    {0,0,0,0,0,1,1,1},  // 25%
    {0,0,0,1,1,1,1,1},  // 50%
    {1,1,1,1,1,1,0,0}   // 75%
};

static const uint8_t length_table[32] = {
    10, 254, 20,  2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
    12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30
};


APU* apu_init() {
    APU* apu = malloc(sizeof(APU));
    if (!apu) {
        return NULL;
    }
    return apu;
}

void api_exit(APU *apu) {
    free(apu);
}

float* pulse_out(float *sample, APU *apu, uint8_t idx, uint32_t cpu_cycles) {
    for (int i=0; i<cpu_cycles; i++) {
        if (apu->pulse[idx].timer == 0) {
            apu->pulse[idx].timer = apu->pulse[idx].m_timer;
            apu->pulse[idx].duty_step = (apu->pulse[idx].duty_step + 1) & 7;
        } else {
            apu->pulse[idx].timer--;
        }

        if (apu->pulse[idx].length_counter)

        if (apu->pulse[idx].timer < 8) {
            sample[i] = 0;
            continue;
        }
        if (apu->pulse[idx].sweep_period < 0x0080) {
            sample[i] = 0;
            continue;
        }

    }
}

bool apu_step(APU* apu, float **out_sample, uint32_t cpu_cycles) {
    uint32_t sample_count = CPU_CLOCK_NTSC / SAMPLE_RATE;
    float *sample[4];
    for (int i=0; i<4; i++)
        sample[i] = malloc(sizeof(float)*sample_count);
    pulse_check_status(apu, 0);
    pulse_out(sample[0], apu, 0, cpu_cycles); 

    pulse_check_status(apu, 1);
    pulse_out(sample[1], apu, 1, cpu_cycles);

    // triangle_out(sample[2], m, cpu_cycles);
    // noise_out(sample[3], m, cpu_cycles);
    // mix(sample);

    for (int i=0; i<4; i++)
        free(sample[i]);
    return false;
}

void apu_write_register(APU*, uint16_t, uint8_t) {

}

uint8_t apu_get_register(APU*, uint16_t) {
    return 0;
}

// check register status, synchronize if changed
void pulse_check_status(APU* apu, uint8_t idx) {
    uint8_t *mem = apu->mem;
    uint16_t mem_start;
    if (idx == 0) mem_start = 0x4000; else  mem_start = 0x4004;

    uint8_t timer = ((mem[mem_start+3] & 0x07) << 8) | mem[mem_start+2];
    uint8_t duty = (mem[mem_start] & 0xC0) >> 6;
    bool length_counter_halt = (bool)((mem[mem_start] & 0x20) >> 5);
    bool volume_envelop_flag = (bool)((mem[mem_start] & 0x10) >> 4);
    uint8_t volume_envelop_period = mem[mem_start] & 0x0F;
    uint8_t length_counter_load = (mem[mem_start+3] & 0xF8) >> 3;
    bool sweep_enable = (bool)((mem[mem_start+1] & 0x80) >> 7);
    uint8_t sweep_period = (mem[mem_start+1] & 0x70) >> 4;
    uint8_t sweep_negate = (mem[mem_start+1] & 0x08) >> 3;
    bool sweep_shift_count = mem[mem_start+1] & 0x07;

    if (timer != apu->pulse[idx].timer) {
        apu->pulse[idx].m_timer = timer;
        apu->pulse[idx].timer = timer;
    }
    if (duty != apu->pulse[idx].duty) {
        apu->pulse[idx].m_duty = timer;
        apu->pulse[idx].duty = timer;
    }
    if (length_counter_halt != apu->pulse[idx].m_length_counter_halt) {
        apu->pulse[idx].m_length_counter_halt = length_counter_halt;
        apu->pulse[idx].length_counter_halt = length_counter_halt;
    }
    if (length_counter_load != apu->pulse[idx].m_length_counter_load) {
        apu->pulse[idx].m_length_counter_load = length_counter_load;
        apu->pulse[idx].length_counter_load = length_counter_load;
    }
    if (volume_envelop_flag != apu->pulse[idx].m_volume_envelope_flag) {
        apu->pulse[idx].m_volume_envelope_flag = volume_envelop_flag;
        apu->pulse[idx].volume_envelope_flag = volume_envelop_flag;
        if (volume_envelop_flag) {
            apu->pulse[idx].envelope_value = 15;
        }
    }
    if (volume_envelop_period != apu->pulse[idx].m_volume_envelope_period) {
        apu->pulse[idx].m_volume_envelope_period = volume_envelop_period;
        apu->pulse[idx].volume_envelope_period = volume_envelop_period;
    }
    if (sweep_enable != apu->pulse[idx].m_sweep_enabled) {
        apu->pulse[idx].m_sweep_enabled = sweep_enable;
        apu->pulse[idx].sweep_enabled = sweep_enable;
    }
    if (sweep_period != apu->pulse[idx].m_sweep_period) {
        apu->pulse[idx].m_sweep_period = sweep_period;
        apu->pulse[idx].sweep_period = sweep_period;
    }
    if (sweep_negate != apu->pulse[idx].m_sweep_negate) {
        apu->pulse[idx].m_sweep_negate = sweep_negate;
        apu->pulse[idx].sweep_negate = sweep_negate;
    }
    if (sweep_shift_count != apu->pulse[idx].m_sweep_shift_count) {
        apu->pulse[idx].m_sweep_shift_count = sweep_shift_count;
        apu->pulse[idx].sweep_shift_count = sweep_shift_count;
    }
}





void triangle_check_status(APU *apu, uint8_t *mem) {

}

void triangle_out(float *sample, uint32_t cpu_cycle) {

}

void noise_check_status(APU *apu, uint8_t *mem) {

}

void noise_out(float *sample, uint32_t cpu_cycle) {

}

float* mix(float* sample[4]) {
    
}