#ifndef MQ135_CORR_H
#define MQ135_CORR_H

#include <Arduino.h>
#include <math.h>

class MQ135_corr {
  public:
    static constexpr float RLOAD = 10.0;    // kΩ
    static constexpr float RZERO = 76.63;   // Valor inicial (calibrar após burn-in)

    MQ135_corr(float vcc = 5.0) {
      _vcc = vcc;
    }

    float getResistance(int adc_corrigido) {
      if (adc_corrigido <= 0 || adc_corrigido >= 1023) return -1;

      float vout = (adc_corrigido / 1023.0) * _vcc;
      return ((_vcc - vout) * RLOAD) / vout;
    }

    float getRZero(int adc_corrigido, float temperature, float humidity) {
      float rs = getResistance(adc_corrigido);
      if (rs <= 0) return -1;

      float corr = getCorrectionFactor(temperature, humidity);
      float ratio = pow((116.6020682 / 400.0), (1.0 / 2.769034857));

      return (rs / corr) * ratio;
    }

    float getPPM(int adc_corrigido) {
      float rs = getResistance(adc_corrigido);
      if (rs <= 0) return 0;

      float ratio = rs / RZERO;
      return 116.6020682 * pow(ratio, -2.769034857);
    }

    float getCorrectedPPM(int adc_corrigido, float temperature, float humidity) {
      float rs = getResistance(adc_corrigido);
      if (rs <= 0) return 0;

      float corr = getCorrectionFactor(temperature, humidity);
      float ratio = (rs / corr) / RZERO;

      return 116.6020682 * pow(ratio, -2.769034857);
    }

  private:
    float _vcc;

    float getCorrectionFactor(float temperature, float humidity) {
      return 0.00035 * temperature * temperature
           - 0.02718 * temperature
           + 1.39538
           - (humidity - 33.0) * 0.0018;
    }
};

#endif // MQ135_CORR_H
