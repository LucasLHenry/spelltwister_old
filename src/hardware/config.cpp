#include <FlashAsEEPROM_SAMD.h>  // 

#include "config.h"

FlashStorage(encoder_flag_storage, char);
FlashStorage(a_idx_storage, int);
FlashStorage(b_idx_storage, int);

FlashStorage(calibration_flag_storage, char);
FlashStorage(a_vo_offset_storage, int);
FlashStorage(b_vo_offset_storage, int);
FlashStorage(a_vo_scale_storage, int);
FlashStorage(b_vo_scale_storage, int);

void update_values_from_config(LedRing& ring, Module& A, Module& B) {
    char c_buf;
    int buf;
    char config_exists_flag = 'S';

    encoder_flag_storage.read(c_buf);
    if (c_buf == config_exists_flag) {
        a_idx_storage.read(buf);
        ring.a_idx = static_cast<uint16_t>(buf);
        ring.a_pos_raw = static_cast<uint16_t>(buf) << ENC_DIV;

        b_idx_storage.read(buf);
        ring.b_idx = static_cast<uint16_t>(buf);
        ring.b_pos_raw = static_cast<uint16_t>(buf) << ENC_DIV;
    } else {
        // DEFAULT VALUES
        ring.a_idx = 0;
        ring.a_pos_raw = 0;

        ring.b_idx = 0;
        ring.b_pos_raw = 0;
    }

    calibration_flag_storage.read(c_buf);
    if (c_buf == config_exists_flag) {
        a_vo_offset_storage.read(buf);
        A.vo_offset = static_cast<uint16_t>(buf);
        a_vo_scale_storage.read(buf);
        A.vo_scale = static_cast<uint16_t>(buf);

        b_vo_offset_storage.read(buf);
        B.vo_offset = static_cast<uint16_t>(buf);
        b_vo_scale_storage.read(buf);
        B.vo_scale = static_cast<uint16_t>(buf);
    } else {
        // DEFAULT VALUES
        A.vo_offset = 155;
        A.vo_scale  = 321;
        B.vo_offset = 148;
        B.vo_scale  = 321;
    }
}

void write_encoder_to_config(LedRing& ring) {
    int buf;
    int out_buf;
    char c_buf;
    char config_exists_flag = 'S';

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
    int buf;
    int out_buf;
    char c_buf;
    char config_exists_flag = 'S';

    a_vo_offset_storage.read(buf);
    if (buf != A.vo_offset) {
        out_buf = A.vo_offset;
        a_vo_offset_storage.write(out_buf);
    }

    a_vo_scale_storage.read(buf);
    if (buf != A.vo_scale) {
        out_buf = A.vo_scale;
        a_vo_scale_storage.write(out_buf);
    }

    b_vo_offset_storage.read(buf);
    if (buf != B.vo_offset) {
        out_buf = B.vo_offset;
        b_vo_offset_storage.write(out_buf);
    }

    b_vo_scale_storage.read(buf);
    if (buf != B.vo_scale) {
        out_buf = B.vo_scale;
        b_vo_scale_storage.write(out_buf);
    }

    calibration_flag_storage.read(c_buf);
    if (c_buf != config_exists_flag) calibration_flag_storage.write(config_exists_flag);
}