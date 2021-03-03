#include <codecvt>
#include <cstdio>
#include <cstdlib>
#include <ctgmath>
#include <regex>
#include <unordered_map>
#include <vector>

#pragma once

namespace NameGen {
	struct Phonemes {
		std::wstring C = L"ptkmnls", V = L"aeiou", S = L"s", F = L"mn", L = L"rl";
		std::wstring operator[](wchar_t) const;
	};

	struct Orthography {
		std::string name = "Default";
		std::unordered_map<wchar_t, std::wstring> map;
	};

	struct Set {
		std::string name;
		std::wstring set;
	};

	struct RegexSet {
		std::string name;
		std::vector<std::wregex> regexen;
	};

	struct Language {
		Phonemes phonemes;
		std::wstring structure = L"CVC";
		size_t exponent = 2;
		std::vector<std::wregex> restricts;
		Orthography cortho, vortho;
		bool noortho = true, nomorph = true, nowordpool = true;
		size_t minsyll = 1, maxsyll = 1;
		std::unordered_map<std::string, std::vector<std::wstring>> morphemes, words;
		std::vector<std::wstring> names;
		std::wstring joiner = L" ";
		size_t maxchar = 12, minchar = 5;
		std::wstring genitive, definite;
		std::wstring spell(const std::wstring &syllable) const;
		std::wstring makeSyllable() const;
		std::wstring getMorpheme(const std::string &key = "");
		std::wstring makeWord(const std::string &key);
		std::wstring getWord(const std::string &key = "");
		std::string makeName(const std::string &key = "");
	};

	std::string convert(const std::wstring &);
	size_t randrange(size_t low, size_t high);
	size_t randrange(size_t high);
	std::wstring join(const std::vector<std::wstring> &, const std::wstring &sep = {});
	Language makeOrthoLanguage();
	Language makeRandomLanguage();

	extern std::vector<Set> consets;
	extern std::vector<Set> vowsets;
	extern std::vector<Set> lsets;
	extern std::vector<Set> ssets;
	extern std::vector<Set> fsets;
	extern std::vector<std::wstring> syllstructs;
	extern std::vector<RegexSet> ressets;
	extern std::vector<Orthography> corthsets;
	extern std::vector<Orthography> vorthsets;

	template <template <typename...> class C, typename... CArgs, typename T>
	const T & choose(const C<T, CArgs...> &container, size_t exponent = 1) {
		return container.at(floor(pow(static_cast<double>(rand()) / static_cast<double>(RAND_MAX), exponent) * container.size()));
	}

	template <typename C>
	C shuffled(C container) {
		for (size_t i = container.size() - 1; 0 < i; --i)
			std::swap(container.at(i), container.at(randrange(i)));
		return container;
	}

	template <typename T>
	std::basic_string<T> capitalize(std::basic_string<T> word) {
		if (!word.empty())
			word.at(0) = std::toupper(word.at(0));
		return word;
	}
}
