#pragma once
#include <vector>
#include "PacketType.h"
#define DEFAULT_SIZE 100
#define NUM_BIT_PACKET 4

namespace PNet
{
	class Packet
	{
	private:
		void write_bits_(const uint32_t data, const size_t bits);
		uint32_t read_bits_(const size_t bit_index, const size_t num_bits, size_t ret);
	public:
		std::vector<uint8_t> _buffer;
		uint32_t _pos;
		uint8_t _bit_index;

		Packet(PacketType packet = PacketType::PT_Invalid, const size_t size = DEFAULT_SIZE);
		~Packet();
		
		/*
		 * Appends the data parameter's bits to the buffer (bytes array)
		 */
		template <typename T>
		inline void write_bits(const T& data, const size_t num_bits) {
			this->write_bits_(static_cast<uint32_t>(data), num_bits);
		}
		uint8_t read_bit(const size_t bit_index);
		uint32_t read_bits(int &bit_index, const size_t num_bits);
		size_t get_size() { return this->_buffer.size(); }
		PacketType GetPacketType();
	};
}