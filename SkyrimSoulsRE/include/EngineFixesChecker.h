#pragma once

#include "../../../OrbisUtil/Third-Party/ToruNiina/toml.hpp"
// #include "AutoTOML.hpp"

namespace SkyrimSoulsRE
{
	// /*
    class EngineFixesConfig
    {
	public:
		struct bSetting
		{
		public:
			bSetting(const char* a_group, const char* a_key, bool a_value) :
				_group(a_group),
				_key(a_key),
				_value(a_value)
			{}

			~bSetting() = default;

			[[nodiscard]] constexpr bool& get() noexcept { return _value; }
			[[nodiscard]] constexpr const bool& get() const noexcept { return _value; }
		public:
			toml::string _group;
			toml::string _key;
			bool _value;
		};
    public:
        // Patches
        static inline bSetting patchMemoryManager{ "Patches", "MemoryManager", true };

        // Fixes
        static inline bSetting fixGlobalTime{ "Fixes", "GlobalTime", true };

        static bool load_config(const std::string& a_path)
        {
			//
			FILE* config = nullptr;
			
			if (fopen_s(&config, a_path.c_str(), "r") != 0)
				return false;
			else
				fclose(config);

			//
			const auto map = toml::parse(a_path);
			const auto patchesTbl = toml::find(map, "Patches").as_table();
			const auto fixesTbl = toml::find(map, "Fixes").as_table();

			//
			patchMemoryManager._value = patchesTbl.at("MemoryManager").as_boolean();
			fixGlobalTime._value = fixesTbl.at("GlobalTime").as_boolean();

			/*
            try
            {
                const auto table = toml::parse_file(a_path);
                const auto& settings = ISetting::get_settings();
                for (const auto& setting : settings)
                {
                    try
                    {
                        setting->load(table);
                    }
                    catch (const std::exception& e)
                    {
                        // SKSE::log::warn(e.what());
                    }
                }
            }
            catch (const toml::parse_error& e)
            {
                std::ostringstream ss;
                ss
                    << "Error parsing file \'" << *e.source().path
                    << "\':\n"
                    << e.description()
                    << "\n  (" << e.source().begin << ")\n";
                // SKSE::log::error(ss.str());
                return false;
            }
			*/

            return true;
        }
    };
	// */
}