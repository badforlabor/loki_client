

#pragma once


#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"
//#include "cereal/types/map.hpp"
#include "cereal/types/polymorphic.hpp"
#include "loki.h"
namespace cereal
{
	//! Saving for std::map<std::string, std::string>
	template <class Archive> inline
		void save(Archive & ar, std::map<std::string, std::string> const & map)
	{
		for (const auto & i : map)
			ar(cereal::make_nvp(i.first, i.second));
	}

	//! Loading for std::map<std::string, std::string>
	template <class Archive> inline
		void load(Archive & ar, std::map<std::string, std::string> & map)
	{
		map.clear();

		auto hint = map.begin();
		while (true)
		{
			const auto namePtr = ar.getNodeName();

			if (!namePtr)
				break;

			std::string key = namePtr;
			std::string value; ar(value);
			hint = map.emplace_hint(hint, std::move(key), std::move(value));
		}
	}
}
namespace cereal
{
	template<class Archive>
	void serialize(Archive & archive, JsonStream& o)
	{
		archive(cereal::make_nvp("stream", o.Labels));
		archive(cereal::make_nvp("values", o.Values));
	}
}
