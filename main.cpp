#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/crc.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>

using namespace boost::asio;

int main() {
  io_service io;

  serial_port port(io, "COM7");
  port.set_option(serial_port_base::baud_rate(115200));
  port.set_option(serial_port_base::character_size(8));
  port.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));
  port.set_option(serial_port_base::parity(serial_port_base::parity::none));
  port.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));

  // 念のためシリアルポートが開くまで少し待つ
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // エラーチェック用にCRC-IBM-16のチェックサムを付記する必要がある
  boost::crc_optimal<16, 0x8005, 0, 0, true, true> crc;

  // focal power (diopter)の値を指定するモードに設定
  {
    uint8_t mode_cmd[6] = {'M', 'w', 'C', 'A', 0x00, 0x00};

    crc.reset();
    crc.process_bytes(mode_cmd, 4);
    mode_cmd[4] = static_cast<uint8_t>(crc.checksum());
    mode_cmd[5] = static_cast<uint8_t>(crc.checksum() >> 8);

    port.write_some(boost::asio::buffer(mode_cmd));

    // for(int i = 0; i < 6; i++) {
    //   printf("%02x ", mode_cmd[i]);
    // }
    // std::cout << std::endl;
  }

  // focal powerが0.0~4.0に動くようコマンドを送信
  for (int i = 0; i < 40; i++) {
    uint8_t fp_cmd[10] = {0x50, 0x77, 0x44, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    int16_t val = (0.1 * i + 5) * 200;

    fp_cmd[4] = static_cast<uint8_t>(val >> 8);
    fp_cmd[5] = static_cast<uint8_t>(val);

    crc.reset();
    crc.process_bytes(fp_cmd, 8);
    fp_cmd[8] = static_cast<uint8_t>(crc.checksum());
    fp_cmd[9] = static_cast<uint8_t>(crc.checksum() >> 8);

    port.write_some(boost::asio::buffer(fp_cmd));

    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    // for(int i = 0; i < 10; i++) {
    //   printf("\\x%02x", fp_cmd[i]);
    // }
    // std::cout << std::endl;
  }

  port.close();

  return 0;
}

