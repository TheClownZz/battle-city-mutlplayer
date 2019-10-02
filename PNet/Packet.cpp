#include "Packet.h"
#include "Constants.h"
#include<iostream>
namespace PNet
{
	Packet::Packet(PacketType packet , size_t size): _pos(0), _bit_index(0)
	{
		this->_buffer.reserve(size);
		write_bits(packet, NUM_BIT_PACKET);
	}

	Packet::~Packet()
	{
	}

	void Packet::write_bits_(const uint32_t data, const size_t bits)
	{
		if (this->_bit_index == 0 && bits > 0) {
			this->_buffer.push_back(0);
		}

		// Not enough bits in this byte to write all the bits required
		if (this->_bit_index + bits > 8) {
			const uint8_t remainder_bits = this->_bit_index + bits - 8;
			uint32_t remainder_value;
			if (remainder_bits <= 32)
				remainder_value = (data & (0xFFFFFFFF >> (32 - remainder_bits)));
			else
			{
				std::cout << "More than 32 bit" << std::endl;
				return;
			}


			this->_buffer[this->_pos] |= (data >> remainder_bits);

			this->_bit_index = 0;
			this->_pos++;

			// Process the remaining bits in the next byte
			this->write_bits_(remainder_value, remainder_bits);

			return;
		}

		const uint8_t offset = 8 - this->_bit_index - bits;
		this->_buffer[this->_pos] |= (data << offset);
		this->_bit_index += bits;

		if (this->_bit_index == 8) {
			this->_bit_index = 0;
			this->_pos++;
		}
	}
	
	uint32_t Packet::read_bits_(const size_t bit_index, const size_t num_bits, size_t ret)
	{
		if (bit_index + num_bits > this->_buffer.size() * 8) {
			throw;
		}

		uint32_t pos = static_cast<uint32_t>(bit_index / 8);
		uint8_t _bit_indexstart = static_cast<uint8_t>(bit_index - (pos * 8));
		uint32_t _bit_indexend = _bit_indexstart + num_bits - 1;

		// If we exceeded the number of bits that can be read
		// from this byte, then move to the next byte and
		// continue reading the bits from that byte
		if (_bit_indexend >= 8) {
			uint8_t byte = this->_buffer[pos];
			int offset = 8 - num_bits - _bit_indexstart;
			if (offset < 0) {
				uint8_t mask = (0xFF >> _bit_indexstart);
				byte &= mask;
			}
			else {
				byte >>= offset;
			}

			//ret += byte;
			uint32_t bits_read = 8 - _bit_indexstart;
			uint32_t p = num_bits - bits_read;
			offset = 0;
			while (p < num_bits) {
				ret += static_cast<uint32_t>(((byte >> offset) & 0x01) * pow(2, p));
				++p;
				++offset;
			}

			return read_bits_(bit_index + bits_read, num_bits - bits_read, ret);
		}


		// Remove everything in front of the starting bit
		uint8_t byte = this->_buffer[pos];
		if (_bit_indexstart > 0) {
			uint8_t mask = ~(0xFF << (8 - _bit_indexstart));
			byte &= mask;
		}

		byte >>= (8 - num_bits - _bit_indexstart);
		ret += static_cast<uint32_t>(byte);

		return ret;
	}

	uint8_t Packet::read_bit(const size_t bit_index)
	{
		return this->read_bits_(bit_index, 1, 0);
	}
	uint32_t Packet::read_bits(int &bit_index, const size_t num_bits)
	{
		uint32_t value = read_bits_(bit_index, num_bits, 0);
		bit_index += num_bits;
		return value;
	}
	PacketType Packet::GetPacketType()
	{
		int bitIndex = 0;
		PacketType p;
		p = (PacketType)read_bits(bitIndex, NUM_BIT_PACKET);
		return p;
	}
}