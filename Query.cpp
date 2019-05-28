//Arbel Nathan 308366749
#include "Query.h"
#include "TextQuery.h"
#include <memory>
#include <set>
#include <algorithm>
#include <iostream>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <regex>
#include <map>
using namespace std;
////////////////////////////////////////////////////////////////////////////////
vector<string> split(string &s)
{
  stringstream ss(regex_replace(s, regex("(\\s+)"), " "));
  string temp;
  vector<string> ans;
  while (getline(ss, temp, ' '))
  {
    ans.push_back(temp);
  }
  return ans;
}

std::shared_ptr<QueryBase> QueryBase::factory(const string &s)
{
  // cout << "just came: " << s << endl;
  string temp = regex_replace(s, regex("(\\s+)"), " ");
  stringstream quer(regex_replace(temp, regex("^(\\s+)"), ""));
  // cout << "actually: " << quer.str() << endl;
  regex And("[\\w']+ \\bAND\\b [\\w']+");
  regex Or("[\\w']+ \\bOR\\b [\\w']+");
  regex n("[\\w']+ \\d+ [\\w']+");
  regex Not("\\bNOT\\b [\\w']+");
  regex word("[\\w']+[\\s]?");
  if (regex_match(quer.str(), And))
  {
    string first;
    string q;
    string second;
    getline(quer, first, ' ');
    getline(quer, q, ' ');
    getline(quer, second, ' ');
    // cout << first << " AND!!! " << second << endl;

    return std::shared_ptr<QueryBase>(new AndQuery(first, second));
  }
  if (regex_match(quer.str(), Or))
  {
    string first;
    string q;
    string second;
    getline(quer, first, ' ');
    getline(quer, q, ' ');
    getline(quer, second, ' ');
    // cout << first << " OR!!! " << second << endl;

    return std::shared_ptr<QueryBase>(new OrQuery(first, second));
  }

  if (regex_match(quer.str(), Not))
  {
    string first;
    string q;
    getline(quer, q, ' ');
    getline(quer, first, ' ');
    // cout << " NOT!!! " << first << endl;

    return std::shared_ptr<QueryBase>(new NotQuery(first));
  }

  if (regex_match(quer.str(), n))
  {
    string first;
    string q;
    string second;
    getline(quer, first, ' ');
    getline(quer, q, ' ');
    getline(quer, second, ' ');
    // cout << first <<" "<< q <<"!!! " << second << endl;

    return std::shared_ptr<QueryBase>(new NQuery(first, second, stoi(q)));
  }
  if (regex_match(quer.str(), word))
  {
    string first;
    string q;
    string second;
    getline(quer, first, ' ');
    getline(quer, q, ' ');
    getline(quer, second, ' ');
    // cout << first <<" "<< q <<"!!! " << second << endl;

    return std::shared_ptr<QueryBase>(new WordQuery(quer.str()));
  }
  else{
     throw std::invalid_argument("Unrecognized search");
  }
  // cout << "Unrecognized search" << endl;
  // return nullptr;
}
//////////////////////////////////////NOT//////////////////////////////////////////////
QueryResult NotQuery::eval(const TextQuery &text) const
{
  QueryResult result = text.query(query_word);
  auto ret_lines = std::make_shared<std::set<line_no>>();
  auto beg = result.begin(), end = result.end();
  auto sz = result.get_file()->size();

  for (size_t n = 0; n != sz; ++n)
  {
    if (beg == end || *beg != n)
      ret_lines->insert(n);
    else if (beg != end)
      ++beg;
  }
  return QueryResult(rep(), ret_lines, result.get_file());
}

//////////////////////////////////////AND//////////////////////////////////////////////
QueryResult AndQuery::eval(const TextQuery &text) const
{
  QueryResult left_result = text.query(left_query);
  QueryResult right_result = text.query(right_query);

  auto ret_lines = std::make_shared<std::set<line_no>>();
  std::set_intersection(left_result.begin(), left_result.end(),
                        right_result.begin(), right_result.end(),
                        std::inserter(*ret_lines, ret_lines->begin()));

  return QueryResult(rep(), ret_lines, left_result.get_file());
}

//////////////////////////////////////OR//////////////////////////////////////////////
QueryResult OrQuery::eval(const TextQuery &text) const
{
  QueryResult left_result = text.query(left_query);
  QueryResult right_result = text.query(right_query);

  auto ret_lines =
      std::make_shared<std::set<line_no>>(left_result.begin(), left_result.end());

  ret_lines->insert(right_result.begin(), right_result.end());

  return QueryResult(rep(), ret_lines, left_result.get_file());
}
//////////////////////////////////////n//////////////////////////////////////////////
QueryResult NQuery::eval(const TextQuery &text) const
{
  using line_no = std::vector<std::string>::size_type;
  QueryResult AndRes = AndQuery::eval(text);
  auto v = std::make_shared<std::set<line_no>>(AndRes.begin(), AndRes.end());
  string result;
  //cout << "Nq eval:::::::" << endl;
  for (line_no a : *v)
  {
    bool isntInN = true;
    result = *(AndRes.get_file()->begin() + a);
   // cout << result << endl;
    result = regex_replace(result, regex("([^'\\w])"), " ");
    auto vec = split(result);
    //  cout << "after split" << endl;
    //  cout << "before for" << endl;
    for (int i = 0; i < vec.size(); i++)
    {
      //   cout << "in for " << vec.size() << endl;
      if (vec.at(i) == left_query)
      {
        for (int j = 0; j <= dist; j++)
        {
          if (i + j + 1 < vec.size() && vec.at(i + j + 1) == right_query)
          {
            isntInN = false;
          }
        }
      }
      else if (vec.at(i) == right_query)
      {
        for (int j = 0; j <= dist; j++)
        {
          if (i + j + 1 < vec.size() && vec.at(i + j + 1) == left_query)
          {
            isntInN = false;
          }
        }
      }
    }

    if (isntInN)
      v->erase(v->find(a));
    ;
    //  cout << "after for" << endl;
  }
  // cout << "after foreach" << endl;

  // cout << "end" << endl;
  return QueryResult(rep(), v, AndRes.get_file());
}
