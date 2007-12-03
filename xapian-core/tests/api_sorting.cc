/** @file api_sorting.cc
 * @brief tests of MSet sorting
 */
/* Copyright (C) 2007 Olly Betts
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <config.h>

#include "api_sorting.h"

#include <xapian.h>

#include "apitest.h"
#include "testutils.h"

using namespace std;

DEFINE_TESTCASE(sortfunctor1,backend && !remote) {
    Xapian::Enquire enquire(get_database("apitest_sortrel"));
    enquire.set_query(Xapian::Query("woman"));

    {
	const int keys[] = { 3, 1 };
	Xapian::MultiValueSorter sorter(keys, keys + 2);

	enquire.set_sort_by_key(&sorter);
	Xapian::MSet mset = enquire.get_mset(0, 10);
	mset_expect_order(mset, 2, 6, 7, 1, 3, 4, 5, 8, 9);
    }

    {
	Xapian::MultiValueSorter sorter;
	sorter.add(3);
	sorter.add(1, false);

	enquire.set_sort_by_key(&sorter);
	Xapian::MSet mset = enquire.get_mset(0, 10);
	mset_expect_order(mset, 7, 6, 2, 8, 9, 4, 5, 1, 3);
    }

    {
	Xapian::MultiValueSorter sorter;
	sorter.add(13); // Value 13 isn't set.
	sorter.add(3);
	sorter.add(1, false);

	enquire.set_sort_by_key(&sorter);
	Xapian::MSet mset = enquire.get_mset(0, 10);
	mset_expect_order(mset, 7, 6, 2, 8, 9, 4, 5, 1, 3);
    }

    {
	Xapian::MultiValueSorter sorter;
	sorter.add(10); // Value 10 isn't always set.
	sorter.add(1, false);

	enquire.set_sort_by_key(&sorter);
	Xapian::MSet mset = enquire.get_mset(0, 10);
	mset_expect_order(mset, 8, 9, 4, 5, 1, 3, 7, 6, 2);
    }

    return true;
}

/// Test reverse sort functor.
DEFINE_TESTCASE(sortfunctor2,writable && !remote) {
    Xapian::WritableDatabase db = get_writable_database();
    Xapian::Document doc;
    doc.add_term("foo");
    doc.add_value(0, "ABB");
    db.add_document(doc);
    doc.add_value(0, "ABC");
    db.add_document(doc);
    doc.add_value(0, string("ABC", 4));
    db.add_document(doc);
    doc.add_value(0, "ABCD");
    db.add_document(doc);
    doc.add_value(0, "ABC\xff");
    db.add_document(doc);

    Xapian::Enquire enquire(db);
    enquire.set_query(Xapian::Query("foo"));

    {
	Xapian::MultiValueSorter sorter;
	sorter.add(0);
	enquire.set_sort_by_key(&sorter);
	Xapian::MSet mset = enquire.get_mset(0, 10);
	mset_expect_order(mset, 5, 4, 3, 2, 1);
    }

    {
	Xapian::MultiValueSorter sorter;
	sorter.add(0, false);
	enquire.set_sort_by_key(&sorter);
	Xapian::MSet mset = enquire.get_mset(0, 10);
	mset_expect_order(mset, 1, 2, 3, 4, 5);
    }

    {
	Xapian::MultiValueSorter sorter;
	sorter.add(0);
	sorter.add(1);
	enquire.set_sort_by_key(&sorter);
	Xapian::MSet mset = enquire.get_mset(0, 10);
	mset_expect_order(mset, 5, 4, 3, 2, 1);
    }

    {
	Xapian::MultiValueSorter sorter;
	sorter.add(0, false);
	sorter.add(1);
	enquire.set_sort_by_key(&sorter);
	Xapian::MSet mset = enquire.get_mset(0, 10);
	mset_expect_order(mset, 1, 2, 3, 4, 5);
    }

    {
	Xapian::MultiValueSorter sorter;
	sorter.add(0);
	sorter.add(1, false);
	enquire.set_sort_by_key(&sorter);
	Xapian::MSet mset = enquire.get_mset(0, 10);
	mset_expect_order(mset, 5, 4, 3, 2, 1);
    }

    {
	Xapian::MultiValueSorter sorter;
	sorter.add(0, false);
	sorter.add(1, false);
	enquire.set_sort_by_key(&sorter);
	Xapian::MSet mset = enquire.get_mset(0, 10);
	mset_expect_order(mset, 1, 2, 3, 4, 5);
    }

    return true;
}
