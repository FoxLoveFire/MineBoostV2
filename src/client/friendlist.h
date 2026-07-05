// Luanti
// SPDX-License-Identifier: LGPL-2.1-or-later
// MineBoostV2 friend system

#pragma once

#include <set>
#include <string>

// Simple persistent friend list used by the client-side ".friend" commands
// and by the friend ESP renderer. Names are matched case-insensitively but
// stored with their original casing for display purposes.
class FriendList
{
public:
	static FriendList &get();

	// Returns false if the name was already on the list.
	bool add(const std::string &name);

	// Returns false if the name was not on the list.
	bool remove(const std::string &name);

	bool isFriend(const std::string &name) const;

	const std::set<std::string> &getAll() const { return m_friends; }

private:
	FriendList();

	void load();
	void save() const;

	static std::string normalize(const std::string &name);

	std::set<std::string> m_friends;       // normalized (lowercase) names
	std::set<std::string> m_friends_display; // original casing, for listing
	std::string m_path;
};
