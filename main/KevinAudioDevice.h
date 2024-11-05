#ifndef _KEVIN_AUDIO_DEVICE_H
#define _KEVIN_AUDIO_DEVICE_H

#include "AudioDevice.h"
#include <driver/i2c_master.h>
#include <driver/i2s_tdm.h>
#include <esp_codec_dev.h>
#include <esp_codec_dev_defaults.h>


class KevinAudioDevice : public AudioDevice {
public:
    KevinAudioDevice();
    virtual ~KevinAudioDevice();
    void Initialize() override;
    // void SetOutputVolume(int volume) override;

private:
    // i2c_master_bus_handle_t i2c_master_handle_ = nullptr;

    // const audio_codec_data_if_t* data_if_ = nullptr;
    // const audio_codec_ctrl_if_t* out_ctrl_if_ = nullptr;
    // const audio_codec_if_t* out_codec_if_ = nullptr;
    // const audio_codec_ctrl_if_t* in_ctrl_if_ = nullptr;
    // const audio_codec_if_t* in_codec_if_ = nullptr;
    // const audio_codec_gpio_if_t* gpio_if_ = nullptr;

    // esp_codec_dev_handle_t output_dev_ = nullptr;
    // esp_codec_dev_handle_t input_dev_ = nullptr;

    void CreateDuplexChannels() override;
    int Read(int16_t* dest, int samples) override;
    int Write(const int16_t* data, int samples) override;
    
};
// class AudioDevice {
// public:
//     AudioDevice();
//     ~AudioDevice();

//     void Start(int input_sample_rate, int output_sample_rate);
//     void OnInputData(std::function<void(const int16_t*, int)> callback);
//     void OutputData(std::vector<int16_t>& data);

//     int input_sample_rate() const { return input_sample_rate_; }
//     int output_sample_rate() const { return output_sample_rate_; }
//     bool duplex() const { return duplex_; }

// private:
//     bool duplex_ = false;
//     int input_sample_rate_ = 0;
//     int output_sample_rate_ = 0;
//     i2s_chan_handle_t tx_handle_ = nullptr;
//     i2s_chan_handle_t rx_handle_ = nullptr;

//     TaskHandle_t audio_input_task_ = nullptr;
    
//     EventGroupHandle_t event_group_;
//     std::function<void(const int16_t*, int)> on_input_data_;

//     void CreateDuplexChannels();
//     void CreateSimplexChannels();
//     void InputTask();
//     int Read(int16_t* dest, int samples);
//     void Write(const int16_t* data, int samples);
// };
#endif // _KEVIN_AUDIO_DEVICE_H
