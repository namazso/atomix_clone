#define _CRT_SECURE_NO_WARNINGS
#include "json.hpp"
#include "../natomix/src/pch.h"
#include "../natomix/src/map.h"
#include <filesystem>
#include <fstream>
#include <iostream>

// Workaround for where filesystem is in experimental namespace
namespace std
{
	namespace experimental { namespace filesystem {} }
	namespace filesystem { using namespace experimental::filesystem; }
}

using json = nlohmann::json;

static std::map<std::string, std::vector<map>> s_packs;

template <typename T>
void transpose_matrix(T* dst, T* src, unsigned n, unsigned m)
{
  for(auto i = 0u; i < n; ++i)
		for(auto j = 0u; j < m; ++j)
			dst[j * n + i] = src[i * m + j];
}

static auto bond_string_to_flag(const std::string& str) -> std::uint16_t
{
	auto flag = std::uint16_t(0);
	for (const auto c : str)
		flag |= bond_char_to_flag(c);
	return flag;
}

static void from_json(const json& j, atom& a)
{
	a.item_kind = j.at(0).get<std::string>().at(0);
	a.bond_flags = bond_string_to_flag(j.at(1).get<std::string>());
}

static void from_json(const json& j, map& m)
{
	memset(&m, 0, sizeof(map));
	strcpy_s(m.id, j.at("id").get<std::string>().c_str());
	strcpy_s(m.name, j.at("name").get<std::string>().c_str());
	{
		const auto& atoms = j.at("atoms");
		for (auto it = std::begin(atoms); it != std::end(atoms); ++it)
			m.atoms[it.key().at(0)] = it.value().get<atom>();
	}
	{
		auto arena = j.at("arena").get<std::vector<std::string>>();
		constexpr auto arr_size = std::extent_v<decltype(m.arena)>;
		const auto arena_h_size = std::min(arena.size(), arr_size);
		decltype(m.arena) arena_original = { 0 };
		for(auto i = 0u; i < arena_h_size; ++i)
		{
			auto& str = arena.at(i);
			strncpy(arena_original[i], arena.at(i).c_str(), arena.at(i).length());
			for(auto& c : arena_original[i])
				c = c == '.' ? 0 : c;
		}
		transpose_matrix(&m.arena[0][0], &arena_original[0][0], arr_size, arr_size);
	}
	{
		auto molecule = j.at("molecule").get<std::vector<std::string>>();
		constexpr auto arr_size = std::extent_v<decltype(m.molecule)>;
		const auto molecule_h_size = std::min(molecule.size(), arr_size);
		decltype(m.molecule) molecule_original = { 0 };;
		for (auto i = 0u; i < molecule_h_size; ++i)
		{
			auto& str = molecule.at(i);
			strncpy(molecule_original[i], molecule.at(i).c_str(), molecule.at(i).length());
			for (auto& c : molecule_original[i])
				c = c == '.' ? 0 : c;
		}
		transpose_matrix(&m.molecule[0][0], &molecule_original[0][0], arr_size, arr_size);
	}
}

static void try_load_mapset(const std::filesystem::path& path)
{
	try
	{
		auto j = json();
		std::ifstream(path) >> j;
		const auto name = j.at("name").get<std::string>();
		auto& levels = j.at("levels");
		s_packs[name] = levels.get<std::vector<map>>();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
	}
}

int main()
{
	for (auto& entry : std::filesystem::directory_iterator("mapsets"))
		try_load_mapset(entry.path());

	const auto fp = fopen("packs.map", "wb");

	const auto packcount = std::uint32_t(s_packs.size());
	fwrite(&packcount, sizeof(packcount), 1, fp);
	for(const auto& pack : s_packs)
	{
		char name[32];
		memset(name, 0, 32);
		strcpy_s(name, pack.first.c_str());
		fwrite(name, 32, 1, fp);
		auto& levels = pack.second;
		const auto size = std::uint32_t(levels.size());
		fwrite(&size, sizeof(size), 1, fp);
		fwrite(levels.data(),
			sizeof(std::decay_t<decltype(levels)>::value_type), size, fp);
	}

	return 0;
}

