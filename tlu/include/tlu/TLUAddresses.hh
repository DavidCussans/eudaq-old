
namespace tlu {

#define EUDAQ_TLU_REGISTERS   \
  EUDAQ_TLU_REG(BUFFER_DEPTH) \
  EUDAQ_TLU_REG(TIMESTAMP_RESET_BIT) \
  EUDAQ_TLU_REG(TRIGGER_COUNTER_RESET_BIT) \
  EUDAQ_TLU_REG(BUFFER_POINTER_RESET_BIT) \
  EUDAQ_TLU_REG(TRIGGER_FSM_RESET_BIT) \
  EUDAQ_TLU_REG(BEAM_TRIGGER_FSM_RESET_BIT) \
  EUDAQ_TLU_REG(DMA_CONTROLLER_RESET_BIT) \
  EUDAQ_TLU_REG(TRIGGER_SCALERS_RESET_BIT) \
  EUDAQ_TLU_REG(FIRMWARE_ID) \
  EUDAQ_TLU_REG(BASE_ADDRESS) \
  EUDAQ_TLU_REG(FIRMWARE_ID_ADDRESS) \
  EUDAQ_TLU_REG(DUT_BUSY_ADDRESS) \
  EUDAQ_TLU_REG(DUT_RESET_ADDRESS) \
  EUDAQ_TLU_REG(DUT_TRIGGER_ADDRESS) \
  EUDAQ_TLU_REG(DUT_MASK_ADDRESS) \
  EUDAQ_TLU_REG(TRIG_INHIBIT_ADDRESS) \
  EUDAQ_TLU_REG(RESET_REGISTER_ADDRESS) \
  EUDAQ_TLU_REG(INITIATE_READOUT_ADDRESS) \
  EUDAQ_TLU_REG(STATE_CAPTURE_ADDRESS) \
  EUDAQ_TLU_REG(TRIGGER_FSM_STATUS_ADDRESS) \
  EUDAQ_TLU_REG(BEAM_TRIGGER_FSM_STATUS_ADDRESS) \
  EUDAQ_TLU_REG(DMA_STATUS_ADDRESS) \
  EUDAQ_TLU_REG(REGISTERED_BUFFER_POINTER_ADDRESS_BASE) \
  EUDAQ_TLU_REG(REGISTERED_BUFFER_POINTER_ADDRESS_BYTES) \
  EUDAQ_TLU_REG(REGISTERED_BUFFER_POINTER_ADDRESS_0) \
  EUDAQ_TLU_REG(REGISTERED_BUFFER_POINTER_ADDRESS_1) \
  EUDAQ_TLU_REG(REGISTERED_TIMESTAMP_ADDRESS_BASE) \
  EUDAQ_TLU_REG(REGISTERED_TIMESTAMP_ADDRESS_BYTES) \
  EUDAQ_TLU_REG(REGISTERED_TIMESTAMP_ADDRESS_0) \
  EUDAQ_TLU_REG(REGISTERED_TIMESTAMP_ADDRESS_1) \
  EUDAQ_TLU_REG(REGISTERED_TIMESTAMP_ADDRESS_2) \
  EUDAQ_TLU_REG(REGISTERED_TIMESTAMP_ADDRESS_3) \
  EUDAQ_TLU_REG(REGISTERED_TIMESTAMP_ADDRESS_4) \
  EUDAQ_TLU_REG(REGISTERED_TIMESTAMP_ADDRESS_5) \
  EUDAQ_TLU_REG(REGISTERED_TIMESTAMP_ADDRESS_6) \
  EUDAQ_TLU_REG(REGISTERED_TIMESTAMP_ADDRESS_7) \
  EUDAQ_TLU_REG(REGISTERED_TRIGGER_COUNTER_ADDRESS_BASE) \
  EUDAQ_TLU_REG(REGISTERED_TRIGGER_COUNTER_ADDRESS_BYTES) \
  EUDAQ_TLU_REG(REGISTERED_TRIGGER_COUNTER_ADDRESS_0) \
  EUDAQ_TLU_REG(REGISTERED_TRIGGER_COUNTER_ADDRESS_1) \
  EUDAQ_TLU_REG(REGISTERED_TRIGGER_COUNTER_ADDRESS_2) \
  EUDAQ_TLU_REG(REGISTERED_TRIGGER_COUNTER_ADDRESS_3) \
  EUDAQ_TLU_REG(BUFFER_POINTER_ADDRESS_BASE) \
  EUDAQ_TLU_REG(BUFFER_POINTER_ADDRESS_BYTES) \
  EUDAQ_TLU_REG(BUFFER_POINTER_ADDRESS_0) \
  EUDAQ_TLU_REG(BUFFER_POINTER_ADDRESS_1) \
  EUDAQ_TLU_REG(TIMESTAMP_ADDRESS_BASE) \
  EUDAQ_TLU_REG(TIMESTAMP_ADDRESS_BYTES) \
  EUDAQ_TLU_REG(TIMESTAMP_ADDRESS_0) \
  EUDAQ_TLU_REG(TIMESTAMP_ADDRESS_1) \
  EUDAQ_TLU_REG(TIMESTAMP_ADDRESS_2) \
  EUDAQ_TLU_REG(TIMESTAMP_ADDRESS_3) \
  EUDAQ_TLU_REG(TIMESTAMP_ADDRESS_4) \
  EUDAQ_TLU_REG(TIMESTAMP_ADDRESS_5) \
  EUDAQ_TLU_REG(TIMESTAMP_ADDRESS_6) \
  EUDAQ_TLU_REG(TIMESTAMP_ADDRESS_7) \
  EUDAQ_TLU_REG(TRIGGER_COUNTER_ADDRESS_BASE) \
  EUDAQ_TLU_REG(TRIGGER_COUNTER_ADDRESS_BYTES) \
  EUDAQ_TLU_REG(TRIGGER_COUNTER_ADDRESS_0) \
  EUDAQ_TLU_REG(TRIGGER_COUNTER_ADDRESS_1) \
  EUDAQ_TLU_REG(TRIGGER_COUNTER_ADDRESS_2) \
  EUDAQ_TLU_REG(TRIGGER_COUNTER_ADDRESS_3) \
  EUDAQ_TLU_REG(BEAM_TRIGGER_AMASK_ADDRESS) \
  EUDAQ_TLU_REG(BEAM_TRIGGER_OMASK_ADDRESS) \
  EUDAQ_TLU_REG(BEAM_TRIGGER_VMASK_ADDRESS) \
  EUDAQ_TLU_REG(INTERNAL_TRIGGER_INTERVAL) \
  EUDAQ_TLU_REG(BEAM_TRIGGER_IN_ADDRESS) \
  EUDAQ_TLU_REG(DUT_RESET_DEBUG_ADDRESS) \
  EUDAQ_TLU_REG(DUT_DEBUG_TRIGGER_ADDRESS) \
  EUDAQ_TLU_REG(DUT_CLOCK_DEBUG_ADDRESS) \
  EUDAQ_TLU_REG(DUT_LED_ADDRESS) \
  EUDAQ_TLU_REG(DUT_I2C_BUS_SELECT_ADDRESS) \
  EUDAQ_TLU_REG(DUT_I2C_BUS_DATA_ADDRESS) \
  EUDAQ_TLU_REG(CLOCK_SOURCE_SELECT_ADDRESS) \
  EUDAQ_TLU_REG(TRIGGER_IN0_COUNTER_BASE) \
  EUDAQ_TLU_REG(TRIGGER_IN0_COUNTER_BYTES) \
  EUDAQ_TLU_REG(TRIGGER_IN0_COUNTER_0) \
  EUDAQ_TLU_REG(TRIGGER_IN0_COUNTER_1) \
  EUDAQ_TLU_REG(TRIGGER_IN1_COUNTER_BASE) \
  EUDAQ_TLU_REG(TRIGGER_IN1_COUNTER_BYTES) \
  EUDAQ_TLU_REG(TRIGGER_IN1_COUNTER_0) \
  EUDAQ_TLU_REG(TRIGGER_IN1_COUNTER_1) \
  EUDAQ_TLU_REG(TRIGGER_IN2_COUNTER_BASE) \
  EUDAQ_TLU_REG(TRIGGER_IN2_COUNTER_BYTES) \
  EUDAQ_TLU_REG(TRIGGER_IN2_COUNTER_0) \
  EUDAQ_TLU_REG(TRIGGER_IN2_COUNTER_1) \
  EUDAQ_TLU_REG(TRIGGER_IN3_COUNTER_BASE) \
  EUDAQ_TLU_REG(TRIGGER_IN3_COUNTER_BYTES) \
  EUDAQ_TLU_REG(TRIGGER_IN3_COUNTER_0) \
  EUDAQ_TLU_REG(TRIGGER_IN3_COUNTER_1) \
  EUDAQ_TLU_REG(REGISTERED_PARTICLE_COUNTER_ADDRESS_BASE) \
  EUDAQ_TLU_REG(REGISTERED_PARTICLE_COUNTER_ADDRESS_BYTES) \
  EUDAQ_TLU_REG(REGISTERED_PARTICLE_COUNTER_ADDRESS_0) \
  EUDAQ_TLU_REG(REGISTERED_PARTICLE_COUNTER_ADDRESS_1) \
  EUDAQ_TLU_REG(REGISTERED_PARTICLE_COUNTER_ADDRESS_2) \
  EUDAQ_TLU_REG(REGISTERED_PARTICLE_COUNTER_ADDRESS_3) \
  EUDAQ_TLU_REG(I2C_SDA_OUT_BIT) \
  EUDAQ_TLU_REG(I2C_SDA_IN_BIT) \
  EUDAQ_TLU_REG(I2C_SCL_OUT_BIT) \
  EUDAQ_TLU_REG(I2C_SCL_IN_BIT) \
  EUDAQ_TLU_REG(I2C_BUS_MOTHERBOARD) \
  EUDAQ_TLU_REG(I2C_BUS_HDMI) \
  EUDAQ_TLU_REG(I2C_BUS_LEMO) \
  EUDAQ_TLU_REG(I2C_BUS_DISPLAY) \
  EUDAQ_TLU_REG(I2C_BUS_MOTHERBOARD_LED_IO) \
  EUDAQ_TLU_REG(I2C_BUS_MOTHERBOARD_TRIGGER__ENABLE_IPSEL_IO) \
  EUDAQ_TLU_REG(I2C_BUS_MOTHERBOARD_RESET_ENABLE_IO) \
  EUDAQ_TLU_REG(I2C_BUS_MOTHERBOARD_FRONT_PANEL_IO) \
  EUDAQ_TLU_REG(I2C_BUS_MOTHERBOARD_LCD_IO) \
  EUDAQ_TLU_REG(I2C_BUS_LEMO_RELAY_IO) \
  EUDAQ_TLU_REG(I2C_BUS_LEMO_LED_IO) \
  EUDAQ_TLU_REG(I2C_BUS_LEMO_LED_IO_VB) \
  EUDAQ_TLU_REG(I2C_BUS_LEMO_ADC)

#define EUDAQ_TLU_MISSING (unsigned)-1
#define EUDAQ_TLU_REG(r) TLU_ ## r,

  struct TLUAddresses {
    unsigned EUDAQ_TLU_REGISTERS;
    unsigned TLU_SCALERS(int i) {
      return i == 0 ? TLU_TRIGGER_IN0_COUNTER_0
        : i == 1 ? TLU_TRIGGER_IN1_COUNTER_0
        : i == 2 ? TLU_TRIGGER_IN2_COUNTER_0
        : i == 3 ? TLU_TRIGGER_IN3_COUNTER_0
        : EUDAQ_TLU_MISSING;
    };
  };

#undef EUDAQ_TLU_REG

  extern TLUAddresses v0_1, v0_2;

}
