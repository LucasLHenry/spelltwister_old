#include <FlashAsEEPROM_SAMD.h>  // 

#include "config.h"

FlashStorage(encoder_flag_storage, char);
FlashStorage(a_idx_storage, uint16_t);
FlashStorage(b_idx_storage, uint16_t);

FlashStorage(calibration_flag_storage, char);
FlashStorage(a_config_data_storage, ConfigData);
FlashStorage(b_config_data_storage, ConfigData);

FlashStorage(a_vo_offset_storage, int);
FlashStorage(b_vo_offset_storage, int);
FlashStorage(a_vo_scale_storage, int);
FlashStorage(b_vo_scale_storage, int);


const ConfigData a_default_config_data = {177, 321, 570, 570, 570};
const ConfigData b_default_config_data = {169, 321, 570, 570, 574};

char config_exists_flag = 'S';

void update_values_from_config(LedRing& ring, Module& A, Module& B) {
    char c_buf;
    uint16_t idx_buf;
    ConfigData conf_buf;

    encoder_flag_storage.read(c_buf);
    if (c_buf == config_exists_flag) {
        a_idx_storage.read(idx_buf);
        ring.a_idx = static_cast<uint16_t>(idx_buf);
        ring.a_pos_raw = static_cast<uint16_t>(idx_buf) << ENC_DIV;

        b_idx_storage.read(idx_buf);
        ring.b_idx = static_cast<uint16_t>(idx_buf);
        ring.b_pos_raw = static_cast<uint16_t>(idx_buf) << ENC_DIV;
    } else {
        // DEFAULT VALUES
        ring.a_idx = 0;
        ring.a_pos_raw = 0;

        ring.b_idx = 0;
        ring.b_pos_raw = 0;
    }

    calibration_flag_storage.read(c_buf);
    if (c_buf == config_exists_flag) {
        a_config_data_storage.read(conf_buf);
        A.configs = conf_buf;

        b_config_data_storage.read(conf_buf);
        B.configs = conf_buf;
    } else {
        A.configs = a_default_config_data;
        B.configs = b_default_config_data;
    }
}

void write_encoder_to_config(LedRing& ring) {
    uint16_t buf;
    uint16_t out_buf;
    char c_buf;

    a_idx_storage.read(buf);
    if (buf != ring.a_idx) {
        out_buf = ring.a_idx;
        a_idx_storage.write(out_buf);
    }

    b_idx_storage.read(buf);
    if (buf != ring.b_idx) {
        out_buf = ring.b_idx;
        b_idx_storage.write(out_buf);
    }

    encoder_flag_storage.read(c_buf);
    if (c_buf != config_exists_flag) encoder_flag_storage.write(config_exists_flag);
}

void write_calibration_to_config(Module& A, Module& B) {
    ConfigData buf;
    ConfigData out_buf;
    char c_buf;

    a_config_data_storage.read(buf);
    if (!configData_eq(buf, A.configs)) {
        out_buf = A.configs;
        a_config_data_storage.write(out_buf);
    }

    b_config_data_storage.read(buf);
    if (!configData_eq(buf, B.configs)) {
        out_buf = B.configs;
        b_config_data_storage.write(out_buf);
    }

    calibration_flag_storage.read(c_buf);
    if (c_buf != config_exists_flag) calibration_flag_storage.write(config_exists_flag);
}