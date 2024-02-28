#include <iterator>
#include <iostream>
#include <string>
#include <vector>
#include <string_view>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <charconv>
#include <stdexcept>
#include <cstdint>
#include <ranges>
#include <concepts>

#include "catch_amalgamated.hpp"

struct Data
{
	uint64_t id;
	std::string_view name;
	std::vector<std::string_view> connections;
};

struct Out
{
	uint64_t n;
	uint64_t id;
	std::string_view name;

	friend std::tuple<uint64_t, uint64_t, std::string_view> tie(const Out& o) {return std::tie(o.n, o.id, o.name);}
	friend bool operator==(const Out& l, const Out& r) {return tie(l) == tie(r);}
	friend std::ostream& operator<<(std::ostream& os, const Out& o) {
		os << "Out(" << o.n << ", " << o.id << ", " << o.name << ")";
		return os;
	}
};

std::string slurp(std::ifstream& in) {
	std::ostringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

std::string str(std::string_view v)
{
	return std::string(v.data(), v.size());
}

template <typename Iter>
std::string_view sv(Iter f, Iter l)
{
	const auto sz = std::distance(f ,l);
	return std::string_view(f, sz);
}

std::string read_file(std::string_view filename)
{
	std::ifstream in(str(filename));
	return slurp(in);
}

void massert(bool b, std::string msg)
{
	if (not b)
	{
		throw std::runtime_error(msg);
	}
}

template <typename Iter>
Iter find(Iter f, Iter l, typename std::iterator_traits<Iter>::value_type val, std::string msg)
{
	auto it = std::find(f, l, val);
	massert(it != l, msg);
	return it;
}

template <typename Iter, typename Iter2>
Iter find_first_of(Iter f, Iter l, Iter2 b, Iter2 e, std::string msg)
{
	auto it = std::find_first_of(f, l, b, e);
	massert(it != l, msg);
	return it;
}

template <typename Iter, typename Int>
void from_chars(Iter f, Iter l, Int &i, std::string msg)
{
	const std::from_chars_result r = std::from_chars(f, l, i);
	massert(r.ec == std::errc{}, msg);
}

Data parse_line(std::string_view line)
{
	auto p = std::begin(line);
	const auto eol = std::end(line);
	// id
	auto n = find(p, eol, ',', "Missing comma");
	uint64_t id = 0;
	from_chars(p, n, id, "Error parsing id");
	// name
	p=n+1;
	n = find(p, eol, ',', "Missing comma");
	const auto name = sv(p, n);
	// connections
	p=n+1;
	std::vector<std::string_view> connections;
	massert(p != eol, "Unexpected end of line");
	while (true)
	{
		const std::string m = ",\n";
		n = std::find_first_of(p, eol, std::begin(m), std::end(m));
		connections.emplace_back(p, n);
		if (n == eol) break;
		p=n+1;
	}
	return Data {
		.id=id,
		.name=name,
		.connections=std::move(connections),
	};
}

std::vector<Data> parse(std::string_view in)
{
	auto p = std::begin(in);
	const auto end = std::end(in);
	std::vector<Data> vec;
	if (p != end)
	{
		p = find(p, end, '\n', "Error reading header")+1; // ignore header
		while (true) {
			const auto eol = find(p, end, '\n', "Error reading line");
			const auto line = sv(p, eol);
			vec.push_back(parse_line(line));
			p = eol;
			if (++p == end) break;
		}
	}
	return vec;
}

std::vector<Out> algorithms(const std::vector<Data>& v, std::predicate<Data> auto accept, size_t max_items)
{
	std::vector<size_t> idxs;
	idxs.resize(v.size());
	const auto f = std::begin(idxs);
	std::iota(std::begin(idxs), std::end(idxs), 0);
	// filter
	const auto filtered = std::remove_if(std::begin(idxs), std::end(idxs), [&](size_t idx){
		return not accept(v[idx]);
	});
	// take
	const auto limited = std::min(filtered, f+max_items);
	const auto size = std::distance(f, limited);
	std::vector<Out> found;
	// enumerate+reverse
	std::transform(std::rend(idxs)-size, std::rend(idxs), std::back_inserter(found), [&, n=static_cast<uint64_t>(0)](size_t idx)mutable{
		return Out{.n=size-(++n), .id=v[idx].id, .name=v[idx].name};
	});
	return found;
}

TEST_CASE("all ismiths", "") {
	const auto needle = "ismith";
	const auto str = read_file("data.csv");
	const auto data = parse(str);
	auto accept = [=](const Data& d){return std::find(std::begin(d.connections), std::end(d.connections), needle) != std::end(d.connections);};
	const size_t max_items = 99;
	const std::vector<Out> expected = {
		{14, 9808, "howellricardo"},
		{13, 8820, "alexandria72"},
		{12, 7603, "stricklandmary"},
		{11, 7524, "melissa08"},
		{10, 6976, "gweber"},
		{9, 6711, "ryan28"},
		{8, 5195, "abbottjasmine"},
		{7, 5132, "martinproctor"},
		{6, 2831, "joel89"},
		{5, 2452, "cynthia95"},
		{4, 2047, "marcramos"},
		{3, 1354, "christopher75"},
		{2, 976, "stephanie34"},
		{1, 827, "melanievance"},
		{0, 708, "campbelljennifer"},
	};
	BENCHMARK("algorithms") {
		const std::vector<Out> found = algorithms(data, accept, max_items);
		REQUIRE(found == expected);
	};
}
