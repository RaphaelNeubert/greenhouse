#include "sensor.h"

t_sensor_info* init_sensors(){
    t_sensor_info* sensors = malloc(sizeof(t_sensor_info));

    //create and initialize 1-wire bus
    sensors->owb = owb_rmt_initialize(&sensors->driver_info, GPIO_DS18B20, RMT_CHANNEL_1, RMT_CHANNEL_0);
    owb_use_crc(sensors->owb, true); //enable CRC check for ROM code
    printf("address owb: %p\n", sensors->owb);
    printf("address driver info: %p\n", &sensors->driver_info);
   
    //find connected devices
    bool found = false;
    sensors->num_devices=0;
    OneWireBus_ROMCode device_codes[MAX_DEVICES] = {0};
    OneWireBus_SearchState search_state = {0};
    owb_search_first(sensors->owb, &search_state, &found);
    while (found){
        char rom_code[17];
        owb_string_from_rom_code(search_state.rom_code, rom_code, sizeof(rom_code));
        printf("  %d : %s\n", sensors->num_devices, rom_code);
        device_codes[sensors->num_devices] = search_state.rom_code;
        sensors->num_devices++;
        owb_search_next(sensors->owb, &search_state, &found);
    }
    printf("number connected devices: %d\n", sensors->num_devices);

    //create DS18B20 devices on 1-wire bus
    for (int i=0; i<sensors->num_devices; i++){
        DS18B20_Info* ds18b20_info = ds18b20_malloc();
        ds18b20_init(ds18b20_info, sensors->owb, device_codes[i]);
        ds18b20_use_crc(ds18b20_info, true);           // enable CRC check on all reads
        ds18b20_set_resolution(ds18b20_info, DS18B20_RESOLUTION);
        sensors->devices[i] = ds18b20_info;
    }
    return sensors;
}

int measure_temp(t_sensor_info* sensors, float* readings){
    //start conversions and wait until finished
    puts("0");
    printf("address owb: %p\n", sensors->owb);
    ds18b20_convert_all(sensors->owb);
    puts("1");
    ds18b20_wait_for_conversion(sensors->devices[0]);
    puts("2");

    //float readings[MAX_DEVICES] = {0};
    DS18B20_ERROR errors[MAX_DEVICES] = {0};
    puts("3");

    for (int i=0; i<sensors->num_devices; i++){
        errors[i] = ds18b20_read_temp(sensors->devices[i], &readings[i]);
    }
    return 0;
}
