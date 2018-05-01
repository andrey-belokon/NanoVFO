void vfo_set_freq(long f1, long f2, long f3)
{
#ifdef VFO_SI570  
  #ifdef VFO_SI5351
    vfo570.set_freq(f1);
    vfo5351.set_freq(f2,f3,0);
  #else
    // single Si570
    if (f1 != 0) vfo570.set_freq(f1);
    else vfo570.set_freq(f3);
  #endif
#else
  #ifdef VFO_SI5351
    vfo5351.set_freq(f1,f2,f3);
  #endif
#endif
}

void UpdateFreq() 
{
  uint8_t cwtx = trx.TX && trx.CWTX;

#ifdef MODE_DC
  vfo_set_freq(
    cwtx? 0: CLK0_MULT*trx.Freq,
    0,
    cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
  );
#endif

#ifdef MODE_DC_QUADRATURE
  vfo5351.set_freq_quadrature(
    cwtx? 0: trx.Freq,
    cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
  );
#endif

#ifdef MODE_SINGLE_IF
  #if defined(SSBDetectorFreq_USB) && defined(SSBDetectorFreq_LSB)
    vfo_set_freq( // инверсия боковой - гетеродин сверху
      #ifdef CWTX_DIRECT_FREQ
        cwtx? 0: 
      #endif
      CLK0_MULT*(trx.Freq + (trx.sideband == LSB ? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]) : (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]))),
      cwtx? 0: CLK1_MULT*(trx.sideband == LSB ? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]) : (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])),
      #ifdef CWTX_DIRECT_FREQ
        cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
      #else
        cwtx? (trx.sideband == LSB ? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])+Settings[ID_CW_TONE_HZ]: (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])-Settings[ID_CW_TONE_HZ]): 0
      #endif
    );
  #elif defined(SSBDetectorFreq_USB)
    long f = trx.Freq;
    if (trx.sideband == LSB) {
      f+=(SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]);
    } else {
      f = abs((SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])-f);
    }
    vfo_set_freq(
      #ifdef CWTX_DIRECT_FREQ
        cwtx? 0: 
      #endif
      CLK0_MULT*f,
      cwtx? 0: CLK1_MULT*(SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]),
      #ifdef CWTX_DIRECT_FREQ
        cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
      #else
        cwtx? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])+Settings[ID_CW_TONE_HZ]: 0
      #endif
    );
  #elif defined(SSBDetectorFreq_LSB)
    long f = trx.Freq;
    if (trx.sideband == USB) {
      f+=(SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]);
    } else {
      f = abs((SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])-f);
    }
    vfo_set_freq(
      #ifdef CWTX_DIRECT_FREQ
        cwtx? 0: 
      #endif
      CLK0_MULT*f,
      cwtx? 0: CLK1_MULT*(SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]),
      #ifdef CWTX_DIRECT_FREQ
        cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
      #else
        cwtx? (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])-Settings[ID_CW_TONE_HZ]: 0
      #endif
    );
  #else
    #error You must define (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]) or/and (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])
  #endif 
#endif

#ifdef MODE_SINGLE_IF_RXTX
  #if defined(SSBDetectorFreq_USB) && defined(SSBDetectorFreq_LSB)
    long f = CLK1_MULT*(trx.sideband == LSB ? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]) : (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]));
    vfo_set_freq( // инверсия боковой - гетеродин сверху
      #ifdef CWTX_DIRECT_FREQ
        cwtx? 0: 
      #endif
      CLK0_MULT*(trx.Freq + (trx.sideband == LSB ? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]) : (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]))),
      trx.TX ? 0 : f,
      #ifdef CWTX_DIRECT_FREQ
        cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
      #else
        cwtx? (trx.sideband == LSB ? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])+Settings[ID_CW_TONE_HZ]: (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])-Settings[ID_CW_TONE_HZ]): (trx.TX ? f : 0)
      #endif
    );
  #elif defined(SSBDetectorFreq_USB)
    long f = trx.Freq;
    if (trx.sideband == LSB) {
      f+=(SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]);
    } else {
      f = abs((SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])-f);
    }
    vfo_set_freq(
      #ifdef CWTX_DIRECT_FREQ
        cwtx? 0: 
      #endif
      CLK0_MULT*f,
      trx.TX ? 0 : CLK1_MULT*(SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]),
      #ifdef CWTX_DIRECT_FREQ
        cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
      #else
        cwtx? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])+Settings[ID_CW_TONE_HZ]: (trx.TX ? CLK1_MULT*(SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]) : 0)
      #endif
    );
  #elif defined(SSBDetectorFreq_LSB)
    long f = trx.Freq;
    if (trx.sideband == USB) {
      f+=(SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]);
    } else {
      f = abs((SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])-f);
    }
    vfo_set_freq(
      #ifdef CWTX_DIRECT_FREQ
        cwtx? 0: 
      #endif
      CLK0_MULT*f,
      trx.TX ? 0 : CLK1_MULT*(SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]),
      #ifdef CWTX_DIRECT_FREQ
        cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
      #else
        cwtx? (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])-Settings[ID_CW_TONE_HZ]: (trx.TX ? CLK1_MULT*(SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]) : 0)
      #endif
    );
  #else
    #error You must define (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]) or/and (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])
  #endif 
#endif

#ifdef MODE_SINGLE_IF_SWAP
  #if defined(SSBDetectorFreq_USB) && defined(SSBDetectorFreq_LSB)
    long vfo = CLK0_MULT*(trx.Freq + (trx.sideband == LSB ? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]) : (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])));
    long f = CLK1_MULT*(trx.sideband == LSB ? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]) : (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]));
    vfo_set_freq( // инверсия боковой - гетеродин сверху
      cwtx? 0: (trx.TX ? f : vfo),
      #ifdef CWTX_DIRECT_FREQ
        cwtx? 0: 
      #endif
      trx.TX ? vfo : f,
      #ifdef CWTX_DIRECT_FREQ
        cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
      #else
        cwtx? (trx.sideband == LSB ? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])+Settings[ID_CW_TONE_HZ]: (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])-Settings[ID_CW_TONE_HZ]): 0
      #endif
    );
  #elif defined(SSBDetectorFreq_USB)
    long f = trx.Freq;
    if (trx.sideband == LSB) {
      f+=(SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]);
    } else {
      f = abs((SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])-f);
    }
    vfo_set_freq(
      cwtx? 0: (trx.TX ? CLK1_MULT*(SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]) : CLK0_MULT*f),
      #ifdef CWTX_DIRECT_FREQ
        cwtx? 0: 
      #endif
      trx.TX ? CLK0_MULT*f : CLK1_MULT*(SSBDetectorFreq_USB+Settings[ID_USB_SHIFT]),
      #ifdef CWTX_DIRECT_FREQ
        cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
      #else
        cwtx? (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])+Settings[ID_CW_TONE_HZ]: 0
      #endif
    );
  #elif defined(SSBDetectorFreq_LSB)
    long f = trx.Freq;
    if (trx.sideband == USB) {
      f+=(SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]);
    } else {
      f = abs((SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])-f);
    }
    vfo_set_freq(
      cwtx? 0: (trx.TX ? CLK1_MULT*(SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]) : CLK0_MULT*f),
      #ifdef CWTX_DIRECT_FREQ
        cwtx? 0: 
      #endif
      trx.TX ? CLK0_MULT*f : CLK1_MULT*(SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]),
      #ifdef CWTX_DIRECT_FREQ
        cwtx? trx.Freq+(trx.sideband == LSB ? -Settings[ID_CW_TONE_HZ]: Settings[ID_CW_TONE_HZ]): 0
      #else
        cwtx? (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT])-Settings[ID_CW_TONE_HZ]: 0
      #endif
    );
  #else
    #error You must define (SSBDetectorFreq_LSB+Settings[ID_LSB_SHIFT]) or/and (SSBDetectorFreq_USB+Settings[ID_USB_SHIFT])
  #endif 
#endif
}

