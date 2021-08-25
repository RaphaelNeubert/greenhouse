#include "sensor.h"

int find_sensors(){
    //create and initialize 1-wire bus
    OneWireBus* owb;
    owb_rmt_driver_info driver_info;
    owb = owb_rmt_initialize(&driver_info, GPIO_DS18B20, RMT_CHANNEL_1, RMT_CHANNEL_0);
    owb_use_crc(owb, true); //enable CRC check for ROM code
   
    //find connected devices
    int num_devices = 0;
    bool found = false;
    OneWireBus_ROMCode device_codes[MAX_DEVICES] = {0};
    OneWireBus_SearchState search_state = {0};
    owb_search_first(owb, &search_state, &found);
    while (found){
        char rom_code[17];
        owb_string_from_rom_code(search_state.rom_code, rom_code, sizeof(rom_code));
        printf("  %d : %s\n", num_devices, rom_code);
        device_codes[num_devices] = search_state.rom_code;
        num_devices++;
        owb_search_next(owb, &search_state, &found);
    }
    printf("number connected devices: %d\n", num_devices);

    //create DS18B20 devices on 1-wire bus
    DS18B20_Info* devices[MAX_DEVICES] = {0};
    for (int i=0; i<num_devices; i++){
        DS18B20_Info* ds18b20_info = ds18b20_malloc();
        devices[i] = ds18b20_info;
        ds18b20_init(ds18b20_info, owb, device_codes[i]);
        ds18b20_use_crc(ds18b20_info, true);           // enable CRC check on all reads
        ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION);
    }
    return 0;
}

float* measure_temp(OneWireBus* owb, DS18B20_Info* devices, int num_devices){
    //start conversions and wait until finished
    ds18b20_convert_all(owb);
    ds18b20_wait_for_conversion(devices[0]);

    float readings[MAX_DEVICES] = {0};
    DS18B20_ERROR errors[MAX_DEVICES] = {0};

    for (int i=0; i<num_devices; i++){
        errors[i] = ds18b20_read_temp(devices[i], &readings[i]);
    }

    return readings;
}
