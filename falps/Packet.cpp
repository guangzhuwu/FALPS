#include "Packet.h"
pkt_id_t Packet::id_seed_;

Packet::Packet()
{
	data_ = NULL;
	id_ = id_seed_++;
	is_self_msg_ = false;
	nbytes_=0;
	nbytes_with_overhead_=0;
}
Packet::~Packet()
{
	if (data_)
		delete data_;//delete null is safe
	data_ = NULL;	
	fragment_id_ = static_cast<size_t>(~0);
	fragment_num_ = static_cast<size_t>(~0);

}