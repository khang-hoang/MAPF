//
//  VDMap.cpp
//  MAPF
//
//  Created by Khang Hoang on 10/13/19.
//  Copyright © 2019 Khang Hoang. All rights reserved.
//

#include "Map.hpp"
#include "DTMap.hpp"
#include "earcut.hpp"
#include <cmath>
#include <fstream>
#include <unordered_map>

DTMap::DTMap(int32_t t_width, int32_t t_height) : Map(t_width, t_height) {
}

DTMap::~DTMap() {
    // std::cout << "~Map" << std::endl;
    // for (Obstacle *obs : this->m_list_obstacle) {
    //     delete obs;
    // }
    // delete this->m_graph;
}

void FindIntersection(
    Point p1, Point p2, Point p3, Point p4,
    bool &lines_intersect, bool &segments_intersect,
    Point &intersection,
    Point &close_p1, Point &close_p2)
{
    // Get the segments' parameters.
    float dx12 = p2.x - p1.x;
    float dy12 = p2.y - p1.y;
    float dx34 = p4.x - p3.x;
    float dy34 = p4.y - p3.y;

    // Solve for t1 and t2
    float denominator = (dy12 * dx34 - dx12 * dy34);

    float t1 = ((p1.x - p3.x) * dy34 + (p3.y - p1.y) * dx34) / denominator;
    if (std::isinf(t1)) {
        // The lines are parallel (or close enough to it).
        lines_intersect = false;
        segments_intersect = false;
        // intersection.x = std::nan();
        // intersection.y = std::nan();
        // close_p1 = new PointF(float.NaN, float.NaN);
        // close_p2 = new PointF(float.NaN, float.NaN);
        return;
    }
    lines_intersect = true;

    float t2 = ((p3.x - p1.x) * dy12 + (p1.y - p3.y) * dx12) / -denominator;
    // Find the point of intersection.
    intersection = Point(p1.x + dx12 * t1, p1.y + dy12 * t1);

    // The segments intersect if t1 and t2 are between 0 and 1.
    segments_intersect =
        ((t1 >= 0) && (t1 <= 1) &&
         (t2 >= 0) && (t2 <= 1));

    // Find the closest points on the segments.
    if (t1 < 0)
    {
        t1 = 0;
    }
    else if (t1 > 1)
    {
        t1 = 1;
    }

    if (t2 < 0)
    {
        t2 = 0;
    }
    else if (t2 > 1)
    {
        t2 = 1;
    }

    close_p1.x = p1.x + dx12 * t1;
    close_p1.y = p1.y + dy12 * t1;
    close_p2.x = p3.x + dx34 * t2;
    close_p2.y = p3.y + dy34 * t2;
}

std::vector<Obstacle*> DTMap::getEnlargedObs(double offset) {
    std::vector<Obstacle*> listObs;
    for (Obstacle* obs : this->m_listObstacle) {
        std::vector<Point> newPoints;
        std::vector<Point> oldPoints = obs->listPoint;
        int num_points = oldPoints.size();
        for (int j = 0; j < num_points; j++) {
            // Find the new location for point j.
            // Find the points before and after j.
            int i = (j - 1);
            if (i < 0) i += num_points;
            int k = (j + 1) % num_points;

            Vector2d v1(oldPoints[j].x - oldPoints[i].x, oldPoints[j].y - oldPoints[i].y);
            v1.x = (v1.x*offset)/v1.length();
            v1.y = (v1.y*offset)/v1.length();

            Vector2d n1(-v1.y, v1.x);

            Point pij1((oldPoints[i].x + n1.x), (oldPoints[i].y + n1.y));
            Point pij2((oldPoints[j].x + n1.x), (oldPoints[j].y + n1.y));

            Vector2d v2(oldPoints[k].x - oldPoints[j].x, oldPoints[k].y - oldPoints[j].y);
            v2.x = (v2.x*offset)/v2.length();
            v2.y = (v2.y*offset)/v2.length();

            Vector2d n2(-v2.y, v2.x);

            Point pjk1((oldPoints[j].x + n2.x), (oldPoints[j].y + n2.y));
            Point pjk2((oldPoints[k].x + n2.x), (oldPoints[k].y + n2.y));

            // See where the shifted lines ij and jk intersect.
            bool lines_intersect, segments_intersect;
            Point poi, close1, close2;
            FindIntersection(pij1, pij2, pjk1, pjk2, lines_intersect, segments_intersect, poi, close1, close2);

            newPoints.push_back(poi);
        }
        listObs.push_back(new Obstacle(newPoints));
    }

    return listObs;
}

void DTMap::constructGraph() {
    for (Vertex* v : this->m_list_vertex) {
        delete v;
    }
    this->m_list_vertex.clear();
    using point_type = std::array<double, 2>;
    std::vector<std::vector<point_type>> polygon;
    std::vector<point_type> outer;
    std::vector<Point> listPoint;
    Point p0 = Point(0, 0);
    Point p1 = Point(this->m_width, 0);
    Point p2 = Point(this->m_width, this->m_height);
    Point p3 = Point(0, this->m_height);
    listPoint.push_back(p0);
    listPoint.push_back(p1);
    listPoint.push_back(p2);
    listPoint.push_back(p3);
    outer.push_back({p0.x, p0.y});
    outer.push_back({p1.x, p1.y});
    outer.push_back({p2.x, p2.y});
    outer.push_back({p3.x, p3.y});
    polygon.push_back(outer);
    std::vector<Obstacle*> newObstacle = getEnlargedObs(15);
    for (Obstacle * obs : newObstacle) {
        std::vector<point_type> inner;
        for (Point& p : obs->listPoint) {
            listPoint.push_back(Point(p.x, p.y));
            inner.push_back({p.x, p.y});
        }
        polygon.push_back(inner);
    }
    std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);
    std::vector<Vertex*> &listVertex = this->m_list_vertex;
    for (Point &p : listPoint) {
        Vertex* v = new Vertex(p.x, p.y);
        listVertex.push_back(v);
    }
    for (int i = 0; i < indices.size(); i+=3) {
        Vertex *v0 = listVertex[indices[i]];
        Vertex *v1 = listVertex[indices[i+1]];
        Vertex *v2 = listVertex[indices[i+2]];
        if (std::find(v0->neighbors.begin(), v0->neighbors.end(), v1) == v0->neighbors.end()) {
            v0->neighbors.push_back(v1);
            v1->neighbors.push_back(v0);
        }
        if (std::find(v1->neighbors.begin(), v1->neighbors.end(), v2) == v1->neighbors.end()) {
            v1->neighbors.push_back(v2);
            v2->neighbors.push_back(v1);
        }
        if (std::find(v2->neighbors.begin(), v2->neighbors.end(), v0) == v2->neighbors.end()) {
            v2->neighbors.push_back(v0);
            v0->neighbors.push_back(v2);
        }
    }
    for (Obstacle *obs : newObstacle) {
        delete obs;
    }
}

