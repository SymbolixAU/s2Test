// Copyright 2017 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS-IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "s2/s2textformat.h"

#include <string>
#include <vector>

//#include <glog/logging.h>
#include "s2/extra/logging.h"

#include "s2/base/stringprintf.h"
#include "s2/strings/serialize.h"
#include "s2/third_party/absl/memory/memory.h"
#include "s2/third_party/absl/strings/str_split.h"
#include "s2/third_party/absl/strings/string_view.h"
#include "s2/third_party/absl/strings/strip.h"
#include "s2/mutable_s2shapeindex.h"
#include "s2/s2latlng.h"
#include "s2/s2lax_polygon_shape.h"
#include "s2/s2lax_polyline_shape.h"
#include "s2/s2loop.h"
#include "s2/s2point_vector_shape.h"
#include "s2/s2polygon.h"
#include "s2/s2polyline.h"

using absl::make_unique;
using absl::string_view;
using std::pair;
using std::unique_ptr;
using std::vector;

namespace s2textformat {

static vector<string_view> SplitString(string_view str, char separator) {
  vector<string_view> result =
      absl::StrSplit(str, separator, absl::SkipWhitespace());
  for (auto& e : result) {
    e = absl::StripAsciiWhitespace(e);
  }
  return result;
}

static bool ParseDouble(const string& str, double* value) {
  char* end_ptr = nullptr;
  *value = strtod(str.c_str(), &end_ptr);
  return end_ptr && *end_ptr == 0;
}

vector<S2LatLng> ParseLatLngsOrDie(string_view str) {
  vector<S2LatLng> latlngs;
  //CHECK(ParseLatLngs(str, &latlngs)) << ": str == \"" << str << "\"";
  CHECK(ParseLatLngs(str, &latlngs));
  return latlngs;
}

bool ParseLatLngs(string_view str, vector<S2LatLng>* latlngs) {
  vector<pair<string, string>> ps;
  if (!DictionaryParse(str, &ps)) return false;
  for (const auto& p : ps) {
    double lat;
    if (!ParseDouble(p.first, &lat)) return false;
    double lng;
    if (!ParseDouble(p.second, &lng)) return false;
    latlngs->push_back(S2LatLng::FromDegrees(lat, lng));
  }
  return true;
}

vector<S2Point> ParsePointsOrDie(string_view str) {
  vector<S2Point> vertices;;
  //CHECK(ParsePoints(str, &vertices)) << ": str == \"" << str << "\"";
 	CHECK(ParsePoints(str, &vertices));
  return vertices;
}

bool ParsePoints(string_view str, vector<S2Point>* vertices) {
  vector<S2LatLng> latlngs;
  if (!ParseLatLngs(str, &latlngs)) return false;
  for (const auto& latlng : latlngs) {
    vertices->push_back(latlng.ToPoint());
  }
  return true;
}

S2Point MakePointOrDie(string_view str) {
  S2Point point;
  //CHECK(MakePoint(str, &point)) << ": str == \"" << str << "\"";
  CHECK(MakePoint(str, &point))
  return point;
}

bool MakePoint(string_view str, S2Point* point) {
  vector<S2Point> vertices;
  if (!ParsePoints(str, &vertices) || vertices.size() != 1) return false;
  *point = vertices[0];
  return true;
}

bool MakeLatLng(string_view str, S2LatLng* latlng) {
  std::vector<S2LatLng> latlngs;
  if (!ParseLatLngs(str, &latlngs) || latlngs.size() != 1) return false;
  *latlng = latlngs[0];
  return true;
}

S2LatLng MakeLatLngOrDie(string_view str) {
  S2LatLng latlng;
  //CHECK(MakeLatLng(str, &latlng)) << ": str == \"" << str << "\"";
  CHECK(MakeLatLng(str, &latlng));
  return latlng;
}

S2LatLngRect MakeLatLngRectOrDie(string_view str) {
  S2LatLngRect rect;
  //CHECK(MakeLatLngRect(str, &rect)) << ": str == \"" << str << "\"";
  CHECK(MakeLatLngRect(str, &rect));
  return rect;
}

bool MakeLatLngRect(string_view str, S2LatLngRect* rect) {
  vector<S2LatLng> latlngs;
  if (!ParseLatLngs(str, &latlngs) || latlngs.empty()) return false;
  *rect = S2LatLngRect::FromPoint(latlngs[0]);
  for (int i = 1; i < latlngs.size(); ++i) {
    rect->AddPoint(latlngs[i]);
  }
  return rect;
}

unique_ptr<S2Loop> MakeLoopOrDie(string_view str) {
  unique_ptr<S2Loop> loop;
  //CHECK(MakeLoop(str, &loop)) << ": str == \"" << str << "\"";
  CHECK(MakeLoop(str, &loop));
  return loop;
}

bool MakeLoop(string_view str, unique_ptr<S2Loop>* loop) {
  if (str == "empty") {
    *loop = make_unique<S2Loop>(S2Loop::kEmpty());
    return true;
  }
  if (str == "full") {
    *loop = make_unique<S2Loop>(S2Loop::kFull());
    return true;
  }
  vector<S2Point> vertices;
  if (!ParsePoints(str, &vertices)) return false;
  *loop = make_unique<S2Loop>(vertices);
  return true;
}

std::unique_ptr<S2Loop> MakeLoop(absl::string_view str) {
  return MakeLoopOrDie(str);
}

unique_ptr<S2Polyline> MakePolylineOrDie(string_view str) {
  unique_ptr<S2Polyline> polyline;
  //CHECK(MakePolyline(str, &polyline)) << ": str == \"" << str << "\"";
  CHECK(MakePolyline(str, &polyline));
  return polyline;
}

bool MakePolyline(string_view str, unique_ptr<S2Polyline>* polyline) {
  vector<S2Point> vertices;
  if (!ParsePoints(str, &vertices)) return false;
  *polyline = make_unique<S2Polyline>(vertices);
  return true;
}

std::unique_ptr<S2Polyline> MakePolyline(absl::string_view str) {
  return MakePolylineOrDie(str);
}

unique_ptr<S2LaxPolylineShape> MakeLaxPolylineOrDie(string_view str) {
  unique_ptr<S2LaxPolylineShape> lax_polyline;
  //CHECK(MakeLaxPolyline(str, &lax_polyline)) << ": str == \"" << str << "\"";
  CHECK(MakeLaxPolyline(str, &lax_polyline));
  return lax_polyline;
}

bool MakeLaxPolyline(string_view str,
                     unique_ptr<S2LaxPolylineShape>* lax_polyline) {
  vector<S2Point> vertices;
  if (!ParsePoints(str, &vertices)) return false;
  *lax_polyline = make_unique<S2LaxPolylineShape>(vertices);
  return true;
}

std::unique_ptr<S2LaxPolylineShape> MakeLaxPolyline(absl::string_view str) {
  return MakeLaxPolylineOrDie(str);
}

static bool InternalMakePolygon(string_view str, bool normalize_loops,
                                unique_ptr<S2Polygon>* polygon) {
  if (str == "empty") str = "";
  vector<string_view> loop_strs = SplitString(str, ';');
  vector<unique_ptr<S2Loop>> loops;
  for (const auto& loop_str : loop_strs) {
    std::unique_ptr<S2Loop> loop;
    if (!MakeLoop(loop_str, &loop)) return false;
    // Don't normalize loops that were explicitly specified as "full".
    if (normalize_loops && !loop->is_full()) loop->Normalize();
    loops.push_back(std::move(loop));
  }
  *polygon = make_unique<S2Polygon>(std::move(loops));
  return true;
}

unique_ptr<S2Polygon> MakePolygonOrDie(string_view str) {
  unique_ptr<S2Polygon> polygon;
  //CHECK(MakePolygon(str, &polygon)) << ": str == \"" << str << "\"";
  CHECK(MakePolygon(str, &polygon));
  return polygon;
}

bool MakePolygon(string_view str, unique_ptr<S2Polygon>* polygon) {
  return InternalMakePolygon(str, true, polygon);
}

std::unique_ptr<S2Polygon> MakePolygon(absl::string_view str) {
  return MakePolygonOrDie(str);
}

unique_ptr<S2Polygon> MakeVerbatimPolygonOrDie(string_view str) {
  unique_ptr<S2Polygon> polygon;
  //CHECK(MakeVerbatimPolygon(str, &polygon)) << ": str == \"" << str << "\"";
  CHECK(MakeVerbatimPolygon(str, &polygon));
  return polygon;
}

bool MakeVerbatimPolygon(string_view str, unique_ptr<S2Polygon>* polygon) {
  return InternalMakePolygon(str, false, polygon);
}

std::unique_ptr<S2Polygon> MakeVerbatimPolygon(absl::string_view str) {
  return MakeVerbatimPolygonOrDie(str);
}

unique_ptr<S2LaxPolygonShape> MakeLaxPolygonOrDie(string_view str) {
  unique_ptr<S2LaxPolygonShape> lax_polygon;
  //CHECK(MakeLaxPolygon(str, &lax_polygon)) << ": str == \"" << str << "\"";
  CHECK(MakeLaxPolygon(str, &lax_polygon));
  return lax_polygon;
}

bool MakeLaxPolygon(string_view str,
                    unique_ptr<S2LaxPolygonShape>* lax_polygon) {
  vector<string_view> loop_strs = SplitString(str, ';');
  vector<vector<S2Point>> loops;
  for (const auto& loop_str : loop_strs) {
    if (loop_str == "full") {
      loops.push_back(vector<S2Point>());
    } else if (loop_str != "empty") {
      vector<S2Point> points;
      if (!ParsePoints(loop_str, &points)) return false;
      loops.push_back(points);
    }
  }
  *lax_polygon = make_unique<S2LaxPolygonShape>(loops);
  return true;
}

std::unique_ptr<S2LaxPolygonShape> MakeLaxPolygon(absl::string_view str) {
  return MakeLaxPolygonOrDie(str);
}

unique_ptr<MutableS2ShapeIndex> MakeIndexOrDie(string_view str) {
  auto index = make_unique<MutableS2ShapeIndex>();
  //CHECK(MakeIndex(str, &index)) << ": str == \"" << str << "\"";
  CHECK(MakeIndex(str, &index));
  return index;
}

bool MakeIndex(string_view str, std::unique_ptr<MutableS2ShapeIndex>* index) {
  vector<string_view> strs = absl::StrSplit(str, '#');
  //DCHECK_EQ(3, strs.size()) << "Must contain two # characters: " << str;

  vector<S2Point> points;
  for (const auto& point_str : SplitString(strs[0], '|')) {
    S2Point point;
    if (!MakePoint(point_str, &point)) return false;
    points.push_back(point);
  }
  if (!points.empty()) {
    (*index)->Add(make_unique<S2PointVectorShape>(std::move(points)));
  }
  for (const auto& line_str : SplitString(strs[1], '|')) {
    std::unique_ptr<S2LaxPolylineShape> lax_polyline;
    if (!MakeLaxPolyline(line_str, &lax_polyline)) return false;
    (*index)->Add(unique_ptr<S2Shape>(lax_polyline.release()));
  }
  for (const auto& polygon_str : SplitString(strs[2], '|')) {
    std::unique_ptr<S2LaxPolygonShape> lax_polygon;
    if (!MakeLaxPolygon(polygon_str, &lax_polygon)) return false;
    (*index)->Add(unique_ptr<S2Shape>(lax_polygon.release()));
  }
  return true;
}

std::unique_ptr<MutableS2ShapeIndex> MakeIndex(absl::string_view str) {
  return MakeIndexOrDie(str);
}

static void AppendVertex(const S2LatLng& ll, string* out) {
  StringAppendF(out, "%.15g:%.15g", ll.lat().degrees(), ll.lng().degrees());
}

static void AppendVertex(const S2Point& p, string* out) {
  S2LatLng ll(p);
  return AppendVertex(ll, out);
}

static void AppendVertices(const S2Point* v, int n, string* out) {
  for (int i = 0; i < n; ++i) {
    if (i > 0) *out += ", ";
    AppendVertex(v[i], out);
  }
}

string ToString(const S2Point& point) {
  string out;
  AppendVertex(point, &out);
  return out;
}

string ToString(const S2LatLng& latlng) {
  string out;
  AppendVertex(latlng, &out);
  return out;
}

string ToString(const S2LatLngRect& rect) {
  string out;
  AppendVertex(rect.lo(), &out);
  out += ", ";
  AppendVertex(rect.hi(), &out);
  return out;
}

string ToString(const S2Loop& loop) {
  if (loop.is_empty()) {
    return "empty";
  } else if (loop.is_full()) {
    return "full";
  }
  string out;
  if (loop.num_vertices() > 0) {
    AppendVertices(&loop.vertex(0), loop.num_vertices(), &out);
  }
  return out;
}

string ToString(const S2Polyline& polyline) {
  string out;
  if (polyline.num_vertices() > 0) {
    AppendVertices(&polyline.vertex(0), polyline.num_vertices(), &out);
  }
  return out;
}

string ToString(const S2Polygon& polygon) {
  if (polygon.is_empty()) {
    return "empty";
  } else if (polygon.is_full()) {
    return "full";
  }
  string out;
  for (int i = 0; i < polygon.num_loops(); ++i) {
    if (i > 0) out += ";\n";
    const S2Loop& loop = *polygon.loop(i);
    AppendVertices(&loop.vertex(0), loop.num_vertices(), &out);
  }
  return out;
}

string ToString(const vector<S2Point>& points) {
  string out;
  AppendVertices(points.data(), points.size(), &out);
  return out;
}

string ToString(const vector<S2LatLng>& latlngs) {
  string out;
  for (int i = 0; i < latlngs.size(); ++i) {
    if (i > 0) out += ", ";
    AppendVertex(latlngs[i], &out);
  }
  return out;
}

string ToString(const S2LaxPolylineShape& polyline) {
  string out;
  if (polyline.num_vertices() > 0) {
    AppendVertices(&polyline.vertex(0), polyline.num_vertices(), &out);
  }
  return out;
}

string ToString(const S2LaxPolygonShape& polygon) {
  string out;
  for (int i = 0; i < polygon.num_loops(); ++i) {
    if (i > 0) out += ";\n";
    int n = polygon.num_loop_vertices(i);
    if (n > 0) AppendVertices(&polygon.loop_vertex(i, 0), n, &out);
  }
  return out;
}

string ToString(const S2ShapeIndex& index) {
  string out;
  for (int dim = 0; dim < 3; ++dim) {
    if (dim > 0) out += "#";
    int count = 0;
    for (int s = 0; s < index.num_shape_ids(); ++s) {
      S2Shape* shape = index.shape(s);
      if (shape == nullptr || shape->dimension() != dim) continue;
      out += (count > 0) ? " | " : (dim > 0) ? " " : "";
      for (int i = 0; i < shape->num_chains(); ++i, ++count) {
        if (i > 0) out += (dim == 2) ? "; " : " | ";
        S2Shape::Chain chain = shape->chain(i);
        AppendVertex(shape->edge(chain.start).v0, &out);
        int limit = chain.start + chain.length;
        if (dim != 1) --limit;
        for (int e = chain.start; e < limit; ++e) {
          out += ", ";
          AppendVertex(shape->edge(e).v1, &out);
        }
      }
    }
    // Example output: "# #", "0:0 # #", "# # 0:0, 0:1, 1:0"
    if (dim == 1 || (dim == 0 && count > 0)) out += " ";
  }
  return out;
}

}  // namespace s2textformat
