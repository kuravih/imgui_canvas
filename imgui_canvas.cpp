#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include <math.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdio.h>
#include <iostream>
#include <sstream>

#include "imgui_canvas.h"
// --------------------------------------------------------------------------------------------------------------------



















// --------------------------------------------------------------------------------------------------------------------
uint ImGuiCanvasShape::nextIndex = 0;
// --------------------------------------------------------------------------------------------------------------------



















// --------------------------------------------------------------------------------------------------------------------
const std::string clipToString(const ImGuiCanvasClip _clip) {
  std::string clipStrings[(uint)ImGuiCanvasClip::COUNT] = IMGUICANVASCLIP_LABELS;
  return clipStrings[(uint)_clip];
}

const bool stringToClip(const std::string _clipString, ImGuiCanvasClip& _clip) {
  std::string clipStrings[(uint)ImGuiCanvasClip::COUNT] = IMGUICANVASCLIP_LABELS;
  auto itShapeClip = std::find(std::begin(clipStrings), std::end(clipStrings), _clipString);
  if (itShapeClip != std::end(clipStrings)) {
    _clip = (ImGuiCanvasClip)(uint)std::distance(std::begin(clipStrings), itShapeClip);
    return true;
  }
  return false;
}
// --------------------------------------------------------------------------------------------------------------------



















// --------------------------------------------------------------------------------------------------------------------
const std::string shapeTypeToString(const ImGuiCanvasShapeType _shapeType) {
  std::string shapeTypeStrings[(uint)ImGuiCanvasShapeType::COUNT] = IMGUICANVASSHAPETYPE_LABELS;
  return shapeTypeStrings[(uint)_shapeType];
}

const bool stringToShapeType(const std::string _shapeTypeString, ImGuiCanvasShapeType& _shapeType) {
  std::string shapeTypeStrings[(uint)ImGuiCanvasShapeType::COUNT] = IMGUICANVASSHAPETYPE_LABELS;
  auto itShapeType = std::find(std::begin(shapeTypeStrings), std::end(shapeTypeStrings), _shapeTypeString);
  if (itShapeType != std::end(shapeTypeStrings)) {
    _shapeType = (ImGuiCanvasShapeType)(uint)std::distance(std::begin(shapeTypeStrings), itShapeType);
    return true;
  }
  return false;
}
// --------------------------------------------------------------------------------------------------------------------



















// ====================================================================================================================
// ---- constructor ---------------------------------------------------------------------------------------------------
ImGuiCanvasShape::ImGuiCanvasShape(std::string _label, const ImVec2 _center, const ImGuiCanvasShapeType _type, const std::initializer_list<float> _parameters, const ImGuiCanvasClip _clip, const bool _drawCtrls) : m_label(_label), m_index(ImGuiCanvasShape::nextIndex++), m_type(_type), m_center(_center), m_clip(_clip), m_drawCtrls(_drawCtrls), m_visible(true) {
  switch (m_type) {
    case ImGuiCanvasShapeType::Circle:
      m_ctrlPoints.push_back(m_center.position + ImVec2(0,*(_parameters.begin())));
      break;
    case ImGuiCanvasShapeType::Ellipse:
      m_ctrlPoints.push_back(m_center.position + ImVec2(*(_parameters.begin()),0));
      m_ctrlPoints.push_back(m_center.position + ImVec2(0,*(_parameters.begin()+1)));
      break;
    case ImGuiCanvasShapeType::Square:
      m_ctrlPoints.push_back(m_center.position + ImVec2(*(_parameters.begin()),0));
      break;
    case ImGuiCanvasShapeType::Rectangle:
      m_ctrlPoints.push_back(m_center.position + ImVec2(*(_parameters.begin()),0));
      m_ctrlPoints.push_back(m_center.position + ImVec2(0,*(_parameters.begin()+1)));
      break;
    case ImGuiCanvasShapeType::HLine:
      m_ctrlPoints.push_back(ImVec2(*(_parameters.begin()),m_center.position.y));
      m_ctrlPoints.push_back(ImVec2(*(_parameters.begin()+1),m_center.position.y));
      break;
    case ImGuiCanvasShapeType::VLine:
      m_ctrlPoints.push_back(ImVec2(m_center.position.x,*(_parameters.begin())));
      m_ctrlPoints.push_back(ImVec2(m_center.position.x,*(_parameters.begin()+1)));
      break;
    default:
      break;
  }
  return;
}
ImGuiCanvasShape::ImGuiCanvasShape(std::string _label, const ImVec2 _center, const ImGuiCanvasShapeType _type, const ImVector<float> _parameters, const ImGuiCanvasClip _clip, const bool _drawCtrls) : m_label(_label), m_index(ImGuiCanvasShape::nextIndex++), m_type(_type), m_center(_center), m_clip(_clip), m_drawCtrls(_drawCtrls), m_visible(true) {
  switch (m_type) {
    case ImGuiCanvasShapeType::Circle:
      m_ctrlPoints.push_back(m_center.position + ImVec2(0,_parameters[0]));
      break;
    case ImGuiCanvasShapeType::Ellipse:
      m_ctrlPoints.push_back(m_center.position + ImVec2(_parameters[0],0));
      m_ctrlPoints.push_back(m_center.position + ImVec2(0,_parameters[1]));
      break;
    case ImGuiCanvasShapeType::Square:
      m_ctrlPoints.push_back(m_center.position + ImVec2(_parameters[0],0));
      break;
    case ImGuiCanvasShapeType::Rectangle:
      m_ctrlPoints.push_back(m_center.position + ImVec2(_parameters[0],0));
      m_ctrlPoints.push_back(m_center.position + ImVec2(0,_parameters[1]));
      break;
    case ImGuiCanvasShapeType::HLine:
      m_ctrlPoints.push_back(ImVec2(_parameters[0],m_center.position.y));
      m_ctrlPoints.push_back(ImVec2(_parameters[1],m_center.position.y));
      break;
    case ImGuiCanvasShapeType::VLine:
      m_ctrlPoints.push_back(ImVec2(m_center.position.x,_parameters[0]));
      m_ctrlPoints.push_back(ImVec2(m_center.position.x,_parameters[1]));
      break;
    default:
      break;
  }
  return;
}
ImGuiCanvasShape::ImGuiCanvasShape(std::string _label, const ImVec2 _center, const ImGuiCanvasShapeType _type, const ImVector<ImVec2> _ctrlPoints, const ImGuiCanvasClip _clip, const bool _drawCtrls) : m_label(_label), m_index(ImGuiCanvasShape::nextIndex++), m_type(_type), m_center(_center), m_clip(_clip), m_drawCtrls(_drawCtrls), m_visible(true) {
  for(ImVec2 ctrlPoint:_ctrlPoints)
    m_ctrlPoints.push_back(ctrlPoint);
}
ImGuiCanvasShape::ImGuiCanvasShape(const ImGuiCanvasShape& _source) : m_label(_source.getLabel()), m_index(_source.getIndex()), m_type(_source.getType()), m_center(_source.getCenter()), m_ctrlPoints(_source.getCtrlPoints()), m_clip(_source.getClip()), m_drawCtrls(_source.getDrawCtrls()), m_visible(_source.getVisible()) {}

// ---- destructor ----------------------------------------------------------------------------------------------------
ImGuiCanvasShape::~ImGuiCanvasShape() {}

// ---- assignment operator -------------------------------------------------------------------------------------------
ImGuiCanvasShape& ImGuiCanvasShape::operator=(const ImGuiCanvasShape& _other) {
  m_color = _other.getColor();
  m_label = _other.getLabel();
  m_type = _other.getType();
  m_index = ImGuiCanvasShape::nextIndex;
  m_center = _other.getCenter();
  m_ctrlPoints = _other.getCtrlPoints();
  m_clip = _other.getClip();
  m_drawCtrls = _other.getDrawCtrls();
  return *this;
}

// --------------------------------------------------------------------------------------------------------------------
std::string ImGuiCanvasShape::toString() const {
  std::stringstream _out;
  _out << getLabel() << "_" << getIndex() << " : (" << getCenter().position.x << "," << getCenter().position.y << ")";
  if (m_ctrlPoints.size() > 0) {
    _out << " {";
    for (const CtrlPoint &ctrlPoint : m_ctrlPoints)
      _out << "(" << ctrlPoint.position.x << ", " << ctrlPoint.position.y << ")";
    _out << "}";
  }
  _out << " " << ((bool)m_clip?"In":"Out");
  _out << " " << (m_drawCtrls?"True":"False");
  return _out.str();
}

// --------------------------------------------------------------------------------------------------------------------
float ImGuiCanvasShape::getAngle(uint _index) const {
  return ImGui::angle(m_center.position, m_ctrlPoints[_index].position);
}

// --------------------------------------------------------------------------------------------------------------------
float ImGuiCanvasShape::getAbsRadius(uint _index) const {
  return ImGui::length(m_center.position, m_ctrlPoints[_index].position);
}

// --------------------------------------------------------------------------------------------------------------------
ImVec2 ImGuiCanvasShape::getRadius(uint _index) const {
  return m_center.position-m_ctrlPoints[_index].position;
}

// --------------------------------------------------------------------------------------------------------------------
void ImGuiCanvasShape::MoveCenter(const ImVec2& _delta) {
  if (getSelected()) {
    m_center.move(_delta);
    switch (m_type) {
      case ImGuiCanvasShapeType::Circle:
        for (int ctrlPointIndex = 0; ctrlPointIndex < m_ctrlPoints.size(); ctrlPointIndex++)
          m_ctrlPoints[ctrlPointIndex].move(_delta);
        break;
      case ImGuiCanvasShapeType::Ellipse:
        for (int ctrlPointIndex = 0; ctrlPointIndex < m_ctrlPoints.size(); ctrlPointIndex++)
          m_ctrlPoints[ctrlPointIndex].move(_delta);
        break;
      case ImGuiCanvasShapeType::Square:
        for (int ctrlPointIndex = 0; ctrlPointIndex < m_ctrlPoints.size(); ctrlPointIndex++)
          m_ctrlPoints[ctrlPointIndex].move(_delta);
        break;
      case ImGuiCanvasShapeType::Rectangle:
        for (int ctrlPointIndex = 0; ctrlPointIndex < m_ctrlPoints.size(); ctrlPointIndex++)
          m_ctrlPoints[ctrlPointIndex].move(_delta);
        break;
      case ImGuiCanvasShapeType::HLine:
        // m_CtrlPoints[_ctrlPointIndex].position.y = m_CtrlPoints[_ctrlPointIndex].position.y + _delta.y;
        // m_Center.position.y = m_CtrlPoints[_ctrlPointIndex].position.y;
        // m_CtrlPoints[0].position.y = m_Center.position.y;
        // m_CtrlPoints[1].position.y = m_Center.position.y;
        break;
      case ImGuiCanvasShapeType::VLine:
        // m_CtrlPoints[_ctrlPointIndex].position.x = m_CtrlPoints[_ctrlPointIndex].position.x + _delta.x;
        // m_Center.position.x = m_CtrlPoints[_ctrlPointIndex].position.x;
        // m_CtrlPoints[0].position.x = m_Center.position.x;
        // m_CtrlPoints[1].position.x = m_Center.position.x;
        break;
      default:
        break;
    }
  }
}

// --------------------------------------------------------------------------------------------------------------------
void ImGuiCanvasShape::MoveCenter(const ImVec2& _delta, const ImVec2& _canvasSize) {
  ImVec2 center, delta;
  if (getSelected()) {
    // ----------------------------------------------------------------------------------------------------------------
    center = m_center.position;
    m_center.move(_delta);
    m_center.moveTo(ImClamp(m_center.position,{0,0},_canvasSize));
    delta = m_center.position - center;
    // ----------------------------------------------------------------------------------------------------------------
    switch (m_type) {
      case ImGuiCanvasShapeType::Circle:
        for (int ctrlPointIndex = 0; ctrlPointIndex < m_ctrlPoints.size(); ctrlPointIndex++)
          m_ctrlPoints[ctrlPointIndex].move(delta);
        break;
      case ImGuiCanvasShapeType::Ellipse:
        for (int ctrlPointIndex = 0; ctrlPointIndex < m_ctrlPoints.size(); ctrlPointIndex++)
          m_ctrlPoints[ctrlPointIndex].move(delta);
        break;
      case ImGuiCanvasShapeType::Square:
        for (int ctrlPointIndex = 0; ctrlPointIndex < m_ctrlPoints.size(); ctrlPointIndex++)
          m_ctrlPoints[ctrlPointIndex].move(delta);
        break;
      case ImGuiCanvasShapeType::Rectangle:
        for (int ctrlPointIndex = 0; ctrlPointIndex < m_ctrlPoints.size(); ctrlPointIndex++)
          m_ctrlPoints[ctrlPointIndex].move(delta);
        break;
      case ImGuiCanvasShapeType::HLine:
        // m_CtrlPoints[_ctrlPointIndex].position.y = m_CtrlPoints[_ctrlPointIndex].position.y + _delta.y;
        // m_Center.position.y = m_CtrlPoints[_ctrlPointIndex].position.y;
        // m_CtrlPoints[0].position.y = m_Center.position.y;
        // m_CtrlPoints[1].position.y = m_Center.position.y;
        break;
      case ImGuiCanvasShapeType::VLine:
        // m_CtrlPoints[_ctrlPointIndex].position.x = m_CtrlPoints[_ctrlPointIndex].position.x + _delta.x;
        // m_Center.position.x = m_CtrlPoints[_ctrlPointIndex].position.x;
        // m_CtrlPoints[0].position.x = m_Center.position.x;
        // m_CtrlPoints[1].position.x = m_Center.position.x;
        break;
      default:
        break;
    }
  }
}

// --------------------------------------------------------------------------------------------------------------------
void ImGuiCanvasShape::MoveCtrlPoint(int _ctrlPointIndex, const ImVec2& _delta) {
  float rectangle_radius, rectangle_angle;
  if (m_ctrlPoints[_ctrlPointIndex].selected) {
    switch (m_type) {
      case ImGuiCanvasShapeType::Circle:
        m_ctrlPoints[_ctrlPointIndex].move(_delta);
        break;
      case ImGuiCanvasShapeType::Ellipse:
        m_ctrlPoints[_ctrlPointIndex].move(_delta);
        if (_ctrlPointIndex == 0) {
          rectangle_radius = getAbsRadius(1);
          rectangle_angle = getAngle(0) + IM_PI/2;
          m_ctrlPoints[1].position = getCenter().position + ImVec2(rectangle_radius*ImCos(rectangle_angle), rectangle_radius*ImSin(rectangle_angle));
        } else {
          rectangle_radius = getAbsRadius(0);
          rectangle_angle = getAngle(1) - IM_PI/2;
          m_ctrlPoints[0].position = getCenter().position + ImVec2(rectangle_radius*ImCos(rectangle_angle), rectangle_radius*ImSin(rectangle_angle));
        }
        break;
      case ImGuiCanvasShapeType::Square:
        m_ctrlPoints[_ctrlPointIndex].move(_delta);
        break;
      case ImGuiCanvasShapeType::Rectangle:
        m_ctrlPoints[_ctrlPointIndex].move(_delta);
        if (_ctrlPointIndex == 0) {
          rectangle_radius = getAbsRadius(1);
          rectangle_angle = getAngle(0) + IM_PI/2;
          m_ctrlPoints[1].position = getCenter().position + ImVec2(rectangle_radius*ImCos(rectangle_angle), rectangle_radius*ImSin(rectangle_angle));
        } else {
          rectangle_radius = getAbsRadius(0);
          rectangle_angle = getAngle(1) - IM_PI/2;
          m_ctrlPoints[0].position = getCenter().position + ImVec2(rectangle_radius*ImCos(rectangle_angle), rectangle_radius*ImSin(rectangle_angle));
        }
        break;
      case ImGuiCanvasShapeType::HLine:
        // m_CtrlPoints[_ctrlPointIndex].position.y = m_CtrlPoints[_ctrlPointIndex].position.y + _delta.y;
        // m_Center.position.y = m_CtrlPoints[_ctrlPointIndex].position.y;
        // m_CtrlPoints[0].position.y = m_Center.position.y;
        // m_CtrlPoints[1].position.y = m_Center.position.y;
        break;
      case ImGuiCanvasShapeType::VLine:
        // m_CtrlPoints[_ctrlPointIndex].position.x = m_CtrlPoints[_ctrlPointIndex].position.x + _delta.x;
        // m_Center.position.x = m_CtrlPoints[_ctrlPointIndex].position.x;
        // m_CtrlPoints[0].position.x = m_Center.position.x;
        // m_CtrlPoints[1].position.x = m_Center.position.x;
        break;
      default:
        break;
    }
  }
}

// --------------------------------------------------------------------------------------------------------------------
void ImGuiCanvasShape::MoveCtrlPoint(int _ctrlPointIndex, const ImVec2& _delta, const ImVec2& _canvasSize) {
  ImVec2 point, delta;
  float rectangle_radius, rectangle_angle;
  if (m_ctrlPoints[_ctrlPointIndex].selected) {
    // ----------------------------------------------------------------------------------------------------------------
    point = m_ctrlPoints[_ctrlPointIndex].position;
    m_ctrlPoints[_ctrlPointIndex].move(_delta);
    m_ctrlPoints[_ctrlPointIndex].moveTo(ImClamp(m_ctrlPoints[_ctrlPointIndex].position,{0,0},_canvasSize));
    delta = m_ctrlPoints[_ctrlPointIndex].position - point;
    // ----------------------------------------------------------------------------------------------------------------
    switch (m_type) {
      case ImGuiCanvasShapeType::Circle:
        m_ctrlPoints[_ctrlPointIndex].move(delta);
        break;
      case ImGuiCanvasShapeType::Ellipse:
        m_ctrlPoints[_ctrlPointIndex].move(delta);
        if (_ctrlPointIndex == 0) {
          rectangle_radius = getAbsRadius(1);
          rectangle_angle = getAngle(0) + IM_PI/2;
          m_ctrlPoints[1].position = getCenter().position + ImVec2(rectangle_radius*ImCos(rectangle_angle), rectangle_radius*ImSin(rectangle_angle));
        } else {
          rectangle_radius = getAbsRadius(0);
          rectangle_angle = getAngle(1) - IM_PI/2;
          m_ctrlPoints[0].position = getCenter().position + ImVec2(rectangle_radius*ImCos(rectangle_angle), rectangle_radius*ImSin(rectangle_angle));
        }
        break;
      case ImGuiCanvasShapeType::Square:
        m_ctrlPoints[_ctrlPointIndex].move(delta);
        break;
      case ImGuiCanvasShapeType::Rectangle:
        m_ctrlPoints[_ctrlPointIndex].move(delta);
        if (_ctrlPointIndex == 0) {
          rectangle_radius = getAbsRadius(1);
          rectangle_angle = getAngle(0) + IM_PI/2;
          m_ctrlPoints[1].position = getCenter().position + ImVec2(rectangle_radius*ImCos(rectangle_angle), rectangle_radius*ImSin(rectangle_angle));
        } else {
          rectangle_radius = getAbsRadius(0);
          rectangle_angle = getAngle(1) - IM_PI/2;
          m_ctrlPoints[0].position = getCenter().position + ImVec2(rectangle_radius*ImCos(rectangle_angle), rectangle_radius*ImSin(rectangle_angle));
        }
        break;
      case ImGuiCanvasShapeType::HLine:
        // m_CtrlPoints[_ctrlPointIndex].position.y = m_CtrlPoints[_ctrlPointIndex].position.y + _delta.y;
        // m_Center.position.y = m_CtrlPoints[_ctrlPointIndex].position.y;
        // m_CtrlPoints[0].position.y = m_Center.position.y;
        // m_CtrlPoints[1].position.y = m_Center.position.y;
        break;
      case ImGuiCanvasShapeType::VLine:
        // m_CtrlPoints[_ctrlPointIndex].position.x = m_CtrlPoints[_ctrlPointIndex].position.x + _delta.x;
        // m_Center.position.x = m_CtrlPoints[_ctrlPointIndex].position.x;
        // m_CtrlPoints[0].position.x = m_Center.position.x;
        // m_CtrlPoints[1].position.x = m_Center.position.x;
        break;
      default:
        break;
    }
  }
}

// --------------------------------------------------------------------------------------------------------------------
bool ImGuiCanvasShape::isInside(const ImVec2& point) const {
  float r0, a0, a;
  ImVec2 d0;
  switch (m_type) {
    case ImGuiCanvasShapeType::Circle:
      return applyClip(getAbsRadius(), ImGui::length(point-m_center.position));
      break;
    case ImGuiCanvasShapeType::Ellipse:
      d0 = (m_center.position - point);
      d0 = ImVec2(d0.x*ImCos(getAngle())+d0.y*ImSin(getAngle()), -d0.x*ImSin(getAngle())+d0.y*ImCos(getAngle()))/ImVec2(getAbsRadius(0), getAbsRadius(1));
      r0 = ImGui::length(d0);
      return applyClip(1,r0);
      break;
    case ImGuiCanvasShapeType::Square:
      r0 = ImGui::length(m_center.position,point);
      a0 = ImGui::angle(m_center.position,point);
      a = -getAngle();
      return applyClip(abs(r0*ImCos(a+a0)), getAbsRadius(), abs(r0*ImSin(a+a0)), getAbsRadius());
      break;
    case ImGuiCanvasShapeType::Rectangle:
      r0 = ImGui::length(m_center.position,point);
      a0 = ImGui::angle(m_center.position,point);
      a = -getAngle();
      return applyClip(abs(r0*ImCos(a+a0)), getAbsRadius(0), abs(r0*ImSin(a+a0)), getAbsRadius(1));
      break;
    case ImGuiCanvasShapeType::HLine:
      return applyClip(m_center.position.y, point.y);
      break;
    case ImGuiCanvasShapeType::VLine:
      return applyClip(m_center.position.x, point.x);
      break;
    default:
      return false;
      break;
  }
}
// ====================================================================================================================
























// ====================================================================================================================
float ImGui::length(const ImVec2& a, const ImVec2& b) {
  return ImGui::length(a-b);
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
float ImGui::length(const ImVec2& a) {
  return ImSqrt(ImLengthSqr(a));
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
float ImGui::angle(const ImVec2& a, const ImVec2& b) {
  return ImGui::angle(b-a);
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
float ImGui::angle(const ImVec2& a) {
  return ImAtan2(a.y, a.x);
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
bool ImGui::DrawShapes(const char* _label, const ImVec2& _origin, const ImVec2& _canvasSize, std::vector<ImGuiCanvasShape>& _shapes, const float _scale, const ImVec2& _size) {
  static bool centerModified = false, ctrlPointModified = false;
  ImVec2 centerDelta, ctrlPointDelta;

  static char centerLabel[128];
  static char ctrlPointLabel[128];
  static ImGuiCanvasShape::CtrlPoint ctrlPoint;
  static ImVec2 transformedCenter, transformedCtrlPoint;
  static float transformedRadius;
  static float angle;
  static float xrate=0.5, xmin=0, xmax=_canvasSize.x;
  static float yrate=0.5, ymin=0, ymax=_canvasSize.y;
  static char centerXLabel[128], centerYLabel[128];
  static ImVec2 previous, next;

  auto vector_transform = [&](const ImVec2& _point) -> ImVec2 {return _origin+_point*_scale;};
  auto scalar_transform = [&](const float& _value) -> float {return _value*_scale;};

  ImGuiWindow* window = GetCurrentWindow();

  for (uint shapeIndex = 0; shapeIndex < _shapes.size(); shapeIndex++) {

    if (!_shapes[shapeIndex].getVisible())
      continue;

    centerModified |= false;
    _shapes[shapeIndex].deselect();
    _shapes[shapeIndex].m_center.deselect();
    
    transformedCenter = vector_transform(ImClamp(_shapes[shapeIndex].m_center.position,{0,0},_size));
    transformedRadius = scalar_transform(_shapes[shapeIndex].getAbsRadius());

    if (_shapes[shapeIndex].getDrawCtrls()) {
      // ---- draw hot area for center control point ------------------------------------------------------------------
      sprintf(centerLabel, "shape#%s#%s#%04d#center", _label, _shapes[shapeIndex].getLabel().c_str(), shapeIndex);
      sprintf(centerXLabel, "##x#%s#%s#%04d#center", _label, _shapes[shapeIndex].getLabel().c_str(), shapeIndex);
      sprintf(centerYLabel, "##y#%s#%s#%04d#center", _label, _shapes[shapeIndex].getLabel().c_str(), shapeIndex);
      SetCursorScreenPos(transformedCenter-HANDLE_2);
      InvisibleButton(centerLabel, HANDLE_4);

      // ---- center control point hover ------------------------------------------------------------------------------
      _shapes[shapeIndex].m_center.setHovered(IsItemActive()||IsItemHovered());
      _shapes[shapeIndex].setHovered(IsItemActive()||IsItemHovered());
      // ---- center control point click ------------------------------------------------------------------------------
      if (IsItemActive()||IsItemClicked()) {
        for (uint shapeIndex = 0; shapeIndex < _shapes.size(); shapeIndex++)
          _shapes[shapeIndex].deselect();
        _shapes[shapeIndex].m_center.select();
        _shapes[shapeIndex].select();
      }

      if (BeginPopupContextItem(centerLabel, ImGuiPopupFlags_MouseButtonRight)) {
        previous = _shapes[shapeIndex].m_center.position;
        next = _shapes[shapeIndex].m_center.position;
        PushItemWidth(40);
        SameLine();
        if (DragScalar(centerXLabel, ImGuiDataType_Float, &next.x, xrate, &xmin, &xmax, "%.0f")) {
          centerDelta = previous - next;
          _shapes[shapeIndex].select();
          _shapes[shapeIndex].MoveCenter(centerDelta, _size);
          centerModified |= true;
          _shapes[shapeIndex].deselect();
        };
        SameLine();
        if (DragScalar(centerYLabel, ImGuiDataType_Float, &next.y, yrate, &ymin, &ymax, "%.0f")) {
          centerDelta = previous - next;
          _shapes[shapeIndex].select();
          _shapes[shapeIndex].MoveCenter(centerDelta, _size);
          centerModified |= true;
          _shapes[shapeIndex].deselect();
        }
        PopItemWidth();
        EndPopup();
      }

      if (_shapes[shapeIndex].m_center.getSelected()&&IsMouseDragging(0)) {
        centerDelta = GetIO().MouseDelta/_scale;
        _shapes[shapeIndex].MoveCenter(centerDelta, _size);
        centerModified |= true;
      }

      // ---- draw center control point -------------------------------------------------------------------------------
      window->DrawList->AddCircle(transformedCenter, MARK_4, _shapes[shapeIndex].m_center.getColor(), 32);
      // --------------------------------------------------------------------------------------------------------------
    }

    // ---- control points loop ---------------------------------------------------------------------------------------
    for (int ctrlPointIndex = 0; ctrlPointIndex < _shapes[shapeIndex].getCtrlPoints().size(); ctrlPointIndex++) {

      if ((_shapes[shapeIndex].getType() == ImGuiCanvasShapeType::HLine) || (_shapes[shapeIndex].getType() == ImGuiCanvasShapeType::VLine))
        continue;

      ctrlPointModified |= false;
      _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();

      if (_shapes[shapeIndex].getDrawCtrls()) {
        // ---- draw hot area for control point -----------------------------------------------------------------------
        transformedCtrlPoint = vector_transform(_shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].position);
        sprintf(ctrlPointLabel, "shape#%s#%s#%04d#ctrlpoint#%04d", _label, _shapes[shapeIndex].getLabel().c_str(), shapeIndex, ctrlPointIndex);
        SetCursorScreenPos(transformedCtrlPoint-HANDLE_2);
        InvisibleButton(ctrlPointLabel, HANDLE_4);

        // ---- control point hover -----------------------------------------------------------------------------------
        _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].setHovered(IsItemActive()||IsItemHovered()||_shapes[shapeIndex].getHovered());
        // ---- control point click -----------------------------------------------------------------------------------
        if (IsItemActive()||IsItemClicked())
          _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();

        if (_shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].getSelected()&&IsMouseDragging(0)) {
          ctrlPointDelta = GetIO().MouseDelta/_scale;
          _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
          ctrlPointModified |= true;
        }

        // ---- draw control point ------------------------------------------------------------------------------------
        window->DrawList->PathArcTo(transformedCtrlPoint, MARK_4, IM_PI/4.0f, IM_PI*7.0f/4.0f, 3);
        window->DrawList->PathStroke(_shapes[shapeIndex].m_center.getSelected()?RED:_shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].getColor(), true);
        // ------------------------------------------------------------------------------------------------------------
      }

      if (_shapes[shapeIndex].getHovered()) {
        SetTooltip("[%.1f,%.1f]",_shapes[shapeIndex].getCenter().position.x, _shapes[shapeIndex].getCenter().position.y);
      }

      if ((!_shapes[shapeIndex].getHovered())&&IsKeyPressed(GetKeyIndex(ImGuiKey_RightArrow))) {
        // right arrow key increases the angle of the hovered control point
        if (_shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].getHovered()) {
          switch (_shapes[shapeIndex].getType()) {
            case ImGuiCanvasShapeType::Circle:
              angle = _shapes[shapeIndex].getAngle() - IM_PI/2.0;
              ctrlPointDelta = ImVec2(ImCos(angle),ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Ellipse:
              angle = _shapes[shapeIndex].getAngle() - IM_PI/2.0;
              ctrlPointDelta = ImVec2(ImCos(angle),ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Square:
              angle = _shapes[shapeIndex].getAngle() - IM_PI/2.0;
              ctrlPointDelta = ImVec2(ImCos(angle),ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Rectangle:
              angle = _shapes[shapeIndex].getAngle() - IM_PI/2.0;
              ctrlPointDelta = ImVec2(ImCos(angle),ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::HLine:
              break;
            case ImGuiCanvasShapeType::VLine:
              break;
            default:
              break;
          }
        }
      }

      if ((!_shapes[shapeIndex].getHovered())&&IsKeyPressed(GetKeyIndex(ImGuiKey_UpArrow))) {
        // up arrow key increases the radius of the hovered control point
        if (_shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].getHovered()) {
          switch (_shapes[shapeIndex].getType()) {
            case ImGuiCanvasShapeType::Circle:
              angle = _shapes[shapeIndex].getAngle();
              ctrlPointDelta = ImVec2(ImCos(angle),ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Ellipse:
              angle = _shapes[shapeIndex].getAngle();
              ctrlPointDelta = ImVec2(ImCos(angle),ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Square:
              angle = _shapes[shapeIndex].getAngle();
              ctrlPointDelta = ImVec2(ImCos(angle),ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Rectangle:
              angle = _shapes[shapeIndex].getAngle();
              ctrlPointDelta = ImVec2(ImCos(angle),ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::HLine:
              break;
            case ImGuiCanvasShapeType::VLine:
              break;
            default:
              break;
          }
        }
      }

      if ((!_shapes[shapeIndex].getHovered())&&IsKeyPressed(GetKeyIndex(ImGuiKey_LeftArrow))) {
        // left arrow key decreases the angle of the hovered control point
        if (_shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].getHovered()) {
          switch (_shapes[shapeIndex].getType()) {
            case ImGuiCanvasShapeType::Circle:
              angle = _shapes[shapeIndex].getAngle() - IM_PI/2.0;
              ctrlPointDelta = ImVec2(-ImCos(angle),-ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Ellipse:
              angle = _shapes[shapeIndex].getAngle() - IM_PI/2.0;
              ctrlPointDelta = ImVec2(-ImCos(angle),-ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Square:
              angle = _shapes[shapeIndex].getAngle() - IM_PI/2.0;
              ctrlPointDelta = ImVec2(-ImCos(angle),-ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Rectangle:
              angle = _shapes[shapeIndex].getAngle() - IM_PI/2.0;
              ctrlPointDelta = ImVec2(-ImCos(angle),-ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::HLine:
              break;
            case ImGuiCanvasShapeType::VLine:
              break;
            default:
              break;
          }
        }
      }

      if ((!_shapes[shapeIndex].getHovered())&&IsKeyPressed(GetKeyIndex(ImGuiKey_DownArrow))) {
        // up arrow key decreases the radius of the hovered control point
        if (_shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].getHovered()) {
          switch (_shapes[shapeIndex].getType()) {
            case ImGuiCanvasShapeType::Circle:
              angle = _shapes[shapeIndex].getAngle();
              ctrlPointDelta = ImVec2(-ImCos(angle),-ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Ellipse:
              angle = _shapes[shapeIndex].getAngle();
              ctrlPointDelta = ImVec2(-ImCos(angle),-ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Square:
              angle = _shapes[shapeIndex].getAngle();
              ctrlPointDelta = ImVec2(-ImCos(angle),-ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::Rectangle:
              angle = _shapes[shapeIndex].getAngle();
              ctrlPointDelta = ImVec2(-ImCos(angle),-ImSin(angle));
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].select();
              _shapes[shapeIndex].MoveCtrlPoint(ctrlPointIndex, ctrlPointDelta, _size);
              ctrlPointModified |= true;
              _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
              break;
            case ImGuiCanvasShapeType::HLine:
              break;
            case ImGuiCanvasShapeType::VLine:
              break;
            default:
              break;
          }
        }
      }

      _shapes[shapeIndex].m_ctrlPoints[ctrlPointIndex].deselect();
    }
    // ----------------------------------------------------------------------------------------------------------------


    angle = _shapes[shapeIndex].getAngle();
    // ---- draw shapes -----------------------------------------------------------------------------------------------
    switch (_shapes[shapeIndex].getType()) {
      case ImGuiCanvasShapeType::Circle:
        AddCircle(window->DrawList, transformedCenter, transformedRadius, _shapes[shapeIndex].getAngle(), _shapes[shapeIndex].getColor());
        break;
      case ImGuiCanvasShapeType::Ellipse:
        AddEllipse(window->DrawList, transformedCenter, _shapes[shapeIndex].getAbsRadius(1)*_scale, _shapes[shapeIndex].getAbsRadius(0)*_scale, _shapes[shapeIndex].getAngle(), _shapes[shapeIndex].getColor());
        break;
      case ImGuiCanvasShapeType::Square:
        AddSquare(window->DrawList, transformedCenter, scalar_transform(_shapes[shapeIndex].getAbsRadius()*ImSqrt(2)), _shapes[shapeIndex].getAngle(), _shapes[shapeIndex].getColor());
        break;
      case ImGuiCanvasShapeType::Rectangle:
        AddRectangle(window->DrawList, transformedCenter, _shapes[shapeIndex].getRadius(1)*_scale, _shapes[shapeIndex].getRadius(0)*_scale, _shapes[shapeIndex].getAngle(), _shapes[shapeIndex].getColor());
        break;
      case ImGuiCanvasShapeType::HLine:
        _shapes[shapeIndex].m_ctrlPoints[0].position.y = _shapes[shapeIndex].m_center.position.y;
        _shapes[shapeIndex].m_ctrlPoints[1].position.y = _shapes[shapeIndex].m_center.position.y;
        AddLine(window->DrawList, vector_transform(_shapes[shapeIndex].m_ctrlPoints[0].position), vector_transform(_shapes[shapeIndex].m_ctrlPoints[1].position), _shapes[shapeIndex].getColor());
        break;
      case ImGuiCanvasShapeType::VLine:
        _shapes[shapeIndex].m_ctrlPoints[0].position.x = _shapes[shapeIndex].m_center.position.x;
        _shapes[shapeIndex].m_ctrlPoints[1].position.x = _shapes[shapeIndex].m_center.position.x;
        AddLine(window->DrawList, vector_transform(_shapes[shapeIndex].m_ctrlPoints[0].position), vector_transform(_shapes[shapeIndex].m_ctrlPoints[1].position), _shapes[shapeIndex].getColor());
        break;
      default:
        break;
    }
    // ----------------------------------------------------------------------------------------------------------------
  
    if (IsKeyPressed(GetKeyIndex(ImGuiKey_RightArrow))) {
      // right arrow moves hovered center to the right 
      if (_shapes[shapeIndex].getHovered()) {
        centerDelta = ImVec2(1,0)/_scale;
        _shapes[shapeIndex].select();
        _shapes[shapeIndex].MoveCenter(centerDelta, _size);
        centerModified |= true;
        _shapes[shapeIndex].deselect();
      }
    }

    if (IsKeyPressed(GetKeyIndex(ImGuiKey_UpArrow))) {
      // up arrow moves hovered center to the up 
      if (_shapes[shapeIndex].getHovered()) {
        centerDelta = ImVec2(0,-1)/_scale;
        _shapes[shapeIndex].select();
        _shapes[shapeIndex].MoveCenter(centerDelta, _size);
        centerModified |= true;
        _shapes[shapeIndex].deselect();
      }
    }

    if (IsKeyPressed(GetKeyIndex(ImGuiKey_LeftArrow))) {
      // left arrow moves hovered center to the left
      if (_shapes[shapeIndex].getHovered()) {
        centerDelta = ImVec2(-1,0)/_scale;
        _shapes[shapeIndex].select();
        _shapes[shapeIndex].MoveCenter(centerDelta, _size);
        centerModified |= true;
        _shapes[shapeIndex].deselect();
      }
    }

    if (IsKeyPressed(GetKeyIndex(ImGuiKey_DownArrow))) {
      // down arrow moves hovered center to the down
      if (_shapes[shapeIndex].getHovered()) {
        centerDelta = ImVec2(0,1)/_scale;
        _shapes[shapeIndex].select();
        _shapes[shapeIndex].MoveCenter(centerDelta, _size);
        centerModified |= true;
        _shapes[shapeIndex].deselect();
      }
    }

    if (IsKeyPressed(GetKeyIndex(ImGuiKey_Delete))) {
      // delete arrow deletes the shape hovered center
      if (_shapes[shapeIndex].getHovered()) {
        _shapes.erase(_shapes.begin()+shapeIndex);
        centerModified |= true;
      }
    }

  }
  return (centerModified|ctrlPointModified);
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
int ImGui::DrawCanvas(const char* _label, const ImVec2& _viewSize, const ImVec2& _canvasSize, std::vector<ImGuiCanvasShape>& _shapes, const ImColor& _borderColor, const ImColor& _bgColor, ImU32 _flags) {
  // no background image
  // draw shapes
  // no mask updating

  bool modified = false;
  static size_t count = 0;
  float scale = _viewSize.x/_canvasSize.x;
  
  if (count != _shapes.size())
    modified = true;

  count = _shapes.size();

  ImGuiWindow* window = GetCurrentWindow();
  ImGuiContext& g = *GImGui;
  const ImGuiStyle& style = g.Style;
  if (window->SkipItems)
    return false;

  ImRect canvas(window->DC.CursorPos, window->DC.CursorPos + _viewSize);

  ItemSize(canvas);
  if (!ItemAdd(canvas, 0))
    return false;

  window->DrawList->AddRectFilled(canvas.Min, canvas.Max, ImU32(_bgColor));
  
  window->DrawList->AddRect(canvas.Min, canvas.Max, ImU32(_borderColor));

  // window->DrawList->PushClipRect(canvas.Min, canvas.Max); // ----------------------------------------------------------

  modified |= DrawShapes(_label, canvas.Min, _canvasSize, _shapes, scale, _canvasSize);

  SetCursorScreenPos(canvas.Min);
  InvisibleButton(_label, canvas.GetSize());
  if (IsItemClicked()) {
    for (uint shapeIndex = 0; shapeIndex < _shapes.size(); shapeIndex++)
      _shapes[shapeIndex].deselect();
  }

  const float GRID_STEP = 64.0*scale;
  for (float x = 0; x < _viewSize.x; x += GRID_STEP)
    window->DrawList->AddLine(ImVec2(canvas.Min.x + x, canvas.Min.y), ImVec2(canvas.Min.x + x, canvas.Max.y), GRAY);
  for (float y = 0; y < _viewSize.y; y += GRID_STEP)
    window->DrawList->AddLine(ImVec2(canvas.Min.x, canvas.Min.y + y), ImVec2(canvas.Max.x, canvas.Min.y + y), GRAY);

  // window->DrawList->PopClipRect(); // ---------------------------------------------------------------------------------

  window->DC.CursorPos = canvas.Max + style.FramePadding;

  return modified;
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
int ImGui::DrawCanvas(const char* _label, const ImVec2& _viewSize, const ImVec2& _canvasSize, std::vector<ImGuiCanvasShape>& _shapes, ImTextureID _textureId, const ImColor& _borderColor, const ImColor& _bgColor, ImU32 _flags) {
  // draw background image
  // draw shapes
  // no mask updating

  bool modified = false;
  static size_t count = 0;
  float scale = _viewSize.x/_canvasSize.x;

  if (count != _shapes.size())
    modified = true;

  count = _shapes.size();

  ImGuiWindow* window = GetCurrentWindow();
  ImGuiContext& g = *GImGui;
  const ImGuiStyle& style = g.Style;
  if (window->SkipItems)
    return false;

  ImRect canvas(window->DC.CursorPos, window->DC.CursorPos + _viewSize);

  ItemSize(canvas);
  if (!ItemAdd(canvas, 0))
    return false;
  
  window->DrawList->AddImage(_textureId, canvas.Min, canvas.Max);

  window->DrawList->AddRect(canvas.Min, canvas.Max, ImU32(_borderColor));

  // window->DrawList->PushClipRect(canvas.Min, canvas.Max); // ----------------------------------------------------------

  modified |= DrawShapes(_label, canvas.Min, _canvasSize, _shapes, scale, _canvasSize);

  SetCursorScreenPos(canvas.Min);
  InvisibleButton(_label, canvas.GetSize());
  if (IsItemClicked()) {
    for (uint shapeIndex = 0; shapeIndex < _shapes.size(); shapeIndex++)
      _shapes[shapeIndex].deselect();
  }

  const float GRID_STEP = 64.0*scale;
  for (float x = 0; x < _viewSize.x; x += GRID_STEP)
    window->DrawList->AddLine(ImVec2(canvas.Min.x + x, canvas.Min.y), ImVec2(canvas.Min.x + x, canvas.Max.y), GRAY);
  for (float y = 0; y < _viewSize.y; y += GRID_STEP)
    window->DrawList->AddLine(ImVec2(canvas.Min.x, canvas.Min.y + y), ImVec2(canvas.Max.x, canvas.Min.y + y), GRAY);

  // window->DrawList->PopClipRect(); // ---------------------------------------------------------------------------------

  window->DC.CursorPos = canvas.Max + style.FramePadding;

  return modified;
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
int ImGui::DrawCanvas(const char* _label, const ImVec2& _viewSize, const ImVec2& _canvasSize, std::vector<ImGuiCanvasShape>& _shapes, ImTextureID _textureId, uint8_t* _mask, bool _updateMask, const ImColor& _borderColor, const ImColor& _bgColor, ImU32 _flags) {
  // draw background image
  // draw shapes
  // draw updating mask

  bool modified = false;
  static size_t count = 0;
  float scale = _viewSize.x/_canvasSize.x;

  if (count != _shapes.size())
    modified = true;

  count = _shapes.size();

  ImGuiWindow* window = GetCurrentWindow();
  ImGuiContext& g = *GImGui;
  const ImGuiStyle& style = g.Style;
  if (window->SkipItems)
    return false;

  ImRect canvas(window->DC.CursorPos, window->DC.CursorPos + _viewSize);

  ItemSize(canvas);
  if (!ItemAdd(canvas, 0))
    return false;
  
  window->DrawList->AddImage(_textureId, canvas.Min, canvas.Max);

  window->DrawList->AddRect(canvas.Min, canvas.Max, ImU32(_borderColor));

  // window->DrawList->PushClipRect(canvas.Min, canvas.Max); // ----------------------------------------------------------

  modified |= DrawShapes(_label, canvas.Min, _canvasSize, _shapes, scale, _canvasSize);

  modified |= _updateMask;

  if (modified)
    UpdateMask(_mask, _canvasSize, _shapes);

  modified = false;

  SetCursorScreenPos(canvas.Min);
  InvisibleButton(_label, canvas.GetSize());
  if (IsItemClicked()) {
    for (uint shapeIndex = 0; shapeIndex < _shapes.size(); shapeIndex++)
      _shapes[shapeIndex].deselect();
  }

  const float GRID_STEP = 64.0*scale;
  for (float x = 0; x < _viewSize.x; x += GRID_STEP)
    window->DrawList->AddLine(ImVec2(canvas.Min.x + x, canvas.Min.y), ImVec2(canvas.Min.x + x, canvas.Max.y), GRAY);
  for (float y = 0; y < _viewSize.y; y += GRID_STEP)
    window->DrawList->AddLine(ImVec2(canvas.Min.x, canvas.Min.y + y), ImVec2(canvas.Max.x, canvas.Min.y + y), GRAY);

  // window->DrawList->PopClipRect(); // ---------------------------------------------------------------------------------

  window->DC.CursorPos = canvas.Max + style.FramePadding;

  return modified;
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
int ImGui::DrawCanvas(const char* _label, const ImVec2& _viewSize, const ImVec2& _canvasSize, ImTextureID _textureId, const ImColor& _borderColor, const ImColor& _bgColor, ImU32 _flags) {
  // draw background image
  // no shapes
  // no updating mask

  bool modified = false;
  float scale = _viewSize.x/_canvasSize.x;


  ImGuiWindow* window = GetCurrentWindow();
  ImGuiContext& g = *GImGui;
  const ImGuiStyle& style = g.Style;
  if (window->SkipItems)
    return false;

  ImRect canvas(window->DC.CursorPos, window->DC.CursorPos + _viewSize);

  ItemSize(canvas);
  if (!ItemAdd(canvas, 0))
    return false;
  
  window->DrawList->AddImage(_textureId, canvas.Min, canvas.Max);

  window->DrawList->AddRect(canvas.Min, canvas.Max, ImU32(_borderColor));

  // window->DrawList->PushClipRect(canvas.Min, canvas.Max); // ----------------------------------------------------------

  modified = false;

  const float GRID_STEP = 64.0*scale;
  for (float x = 0; x < _viewSize.x; x += GRID_STEP)
    window->DrawList->AddLine(ImVec2(canvas.Min.x + x, canvas.Min.y), ImVec2(canvas.Min.x + x, canvas.Max.y), GRAY);
  for (float y = 0; y < _viewSize.y; y += GRID_STEP)
    window->DrawList->AddLine(ImVec2(canvas.Min.x, canvas.Min.y + y), ImVec2(canvas.Max.x, canvas.Min.y + y), GRAY);

  // window->DrawList->PopClipRect(); // ---------------------------------------------------------------------------------

  window->DC.CursorPos = canvas.Max + style.FramePadding;

  return modified;
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
void ImGui::UpdateMask(uint8_t* _mask, const ImVec2& _canvasSize, std::vector<ImGuiCanvasShape>& _shapes) {
  for (int x = 0; x < _canvasSize.x; x++) {
    for (int y = 0; y < _canvasSize.y; y++) {
      _mask[x+(int)(_canvasSize.x*y)] = 0;
      for (ImGuiCanvasShape &shape:_shapes) {
        if (!shape.getVisible())
          continue;
        _mask[x+(int)(_canvasSize.x*y)] |= (int)shape.isInside(ImVec2(x,y));
      }
    }
  }
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
void ImGui::AddCircle(ImDrawList* _ptrDrawList, const ImVec2& _center, float _radius, float _tilt, const ImColor& _color, int _numSegments, float _thickness) {
  _ptrDrawList->AddCircle(_center, _radius, _color, _numSegments, _thickness);

  float IM_PI_2 = IM_PI/2;
  ImVec2 marks[4];
  marks[0] = _center+ImVec2(_radius*ImCos(_tilt), _radius*ImSin(_tilt));
  _ptrDrawList->AddLine(_center, marks[0], _color, _thickness);
  marks[1] = _center+ImVec2(_radius*ImCos(IM_PI_2+_tilt), _radius*ImSin(IM_PI_2+_tilt));
  _ptrDrawList->AddLine(_center, marks[1], _color, _thickness);
  marks[2] = _center+ImVec2(_radius*ImCos(IM_PI+_tilt), _radius*ImSin(IM_PI+_tilt));
  _ptrDrawList->AddLine(_center, marks[2], _color, _thickness);
  marks[3] = _center+ImVec2(_radius*ImCos(3*IM_PI_2+_tilt), _radius*ImSin(3*IM_PI_2+_tilt));
  _ptrDrawList->AddLine(_center, marks[3], _color, _thickness);
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
void ImGui::AddEllipse(ImDrawList* _ptrDrawList, const ImVec2& _center, float _radiusA, float _radiusB, float _tilt, const ImColor& _color, int _numSegments, float _thickness) {
  ImVector<ImVec2> points;
  points.resize(_numSegments);

  float r, theta, dtheta = 2*IM_PI/_numSegments;
  // float ecc2=1-ImPow(_radiusA/_radiusB,2);
  for (int i = 0; i < _numSegments; i++) {
    theta = dtheta*i;
    // theta = theta - (ecc2/8 + ImPow(ecc2,2)/16 + 71*ImPow(ecc2,3)/2048)*ImSin(2*theta) + (5*(ImPow(ecc2,2) + ImPow(ecc2,3))/256)*ImSin(4*theta)  + (29*ImPow(ecc2,3)/6144)*ImSin(6*theta); // https://math.stackexchange.com/questions/2093569/points-on-an-ellipse
    r = _radiusA*_radiusB/ImSqrt(ImPow(_radiusA*ImCos(theta),2) + ImPow(_radiusB*ImSin(theta),2));
    points[i] = _center+ImVec2(r*ImCos(theta+_tilt), r*ImSin(theta+_tilt));
  }

  _ptrDrawList->AddPolyline(points.Data, _numSegments, _color, ImDrawFlags_Closed, _thickness);

  float IM_PI_2 = IM_PI/2;
  ImVec2 marks[4];
  for (int i = 0; i < 4; i++) {
    theta = i*IM_PI_2;
    r = _radiusA*_radiusB/ImSqrt(ImPow(_radiusA*ImCos(theta),2) + ImPow(_radiusB*ImSin(theta),2));
    marks[i] = _center+ImVec2(r*ImCos(theta+_tilt), r*ImSin(theta+_tilt));
    _ptrDrawList->AddLine(_center, marks[i], _color, _thickness);
  }
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
void ImGui::AddRectangle(ImDrawList* _ptrDrawList, const ImVec2& _center, const ImVec2& _sideA, const ImVec2& _sideB, float _tilt, const ImColor& _color, float _thickness) {
  ImVec2 points[4];
  points[0] = _center + ImVec2(   _sideA.x - _sideB.x,   _sideA.y - _sideB.y);
  points[1] = _center + ImVec2( - _sideA.x - _sideB.x, - _sideA.y - _sideB.y);
  points[2] = _center + ImVec2( - _sideA.x + _sideB.x, - _sideA.y + _sideB.y);
  points[3] = _center + ImVec2(   _sideA.x + _sideB.x, + _sideA.y + _sideB.y);
  _ptrDrawList->AddQuad(points[0], points[1], points[2], points[3], _color);
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
void ImGui::AddSquare(ImDrawList* _ptrDrawList, const ImVec2& _center, float _side, float _tilt, const ImColor& _color, float _thickness) {
  _ptrDrawList->PathArcTo(_center, _side, _tilt-IM_PI/4.0f, _tilt+(5.0f*IM_PI/4.0f), 3);
  _ptrDrawList->PathStroke(_color, true);

  float r, theta, IM_PI_2 = IM_PI/2;
  ImVec2 marks[4];
  for (int i = 0; i < 4; i++) {
    theta = i*IM_PI_2;
    r = _side/ImSqrt(2);
    marks[i] = _center+ImVec2(r*ImCos(theta+_tilt), r*ImSin(theta+_tilt));
    _ptrDrawList->AddLine(_center, marks[i], _color, _thickness);
  }
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
void ImGui::AddLine(ImDrawList* _ptrDrawList,  const ImVec2& _point0, const ImVec2& _point1, const ImColor& _color, float _thickness) {
  _ptrDrawList->AddLine(_point0, _point1, _color);
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
void ImGui::AddHLine(ImDrawList* _ptrDrawList,  const ImVec2& _point, float& _left, float& _right, const ImColor& _color, float _thickness) {
  ImVec2 point0 = _point, point1 = _point;
  point0.x = _point.x + _right;
  point1.x = _point.x - _left;
  _ptrDrawList->AddLine(point0, point1, _color, _thickness);
}
// --------------------------------------------------------------------------------------------------------------------










// --------------------------------------------------------------------------------------------------------------------
void ImGui::AddVLine(ImDrawList* _ptrDrawList,  const ImVec2& _point, float& _top, float& _bottom, const ImColor& _color, float _thickness) {
  ImVec2 point0 = _point, point1 = _point;
  point0.y = _point.y + _top;
  point1.y = _point.y - _bottom;
  _ptrDrawList->AddLine(point0, point1, _color, _thickness);
}
// ====================================================================================================================
