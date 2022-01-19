#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui.h"
#include <string>

#define MARK_4 4
#define MARK_2 (MARK_4/2)

const ImVec2 HANDLE_2 = ImVec2(MARK_2, MARK_2);
const ImVec2 HANDLE_4 = ImVec2(MARK_4, MARK_4);
const ImColor WHITE = ImColor(255,255,255,255);
const ImColor RED = ImColor(255,0,0,255);
const ImColor YELLOW = ImColor(255,255,0,255);
const ImColor GRAY = ImColor(200, 200, 200, 40);

#define BASE_VERTEX_SHADER_FILE_STR "shader/base.vertex.glsl"
#define GRAY_FRAGMENT_SHADER_FILE_STR "shader/gray.fragment.glsl"
#define MASK_FRAGMENT_SHADER_FILE_STR "shader/mask.fragment.glsl"

enum class ImGuiCanvasClip : uint8_t {
  In,
  Out,
  COUNT
};
#define IMGUICANVASCLIP_LABELS {"In",\
                                "Out"}

const std::string clipToString(const ImGuiCanvasClip _clip);
const bool stringToClip(const std::string _clipString, ImGuiCanvasClip& _clip);

enum class ImGuiCanvasShapeType : uint8_t {
  Circle,
  Ellipse,
  Square,
  Rectangle,
  HLine,
  VLine,
  COUNT
};
#define IMGUICANVASSHAPETYPE_LABELS {"Circle",\
                                     "Ellipse",\
                                     "Square",\
                                     "Rectangle",\
                                     "HLine",\
                                     "VLine"}

const std::string shapeTypeToString(const ImGuiCanvasShapeType _shapeType);
const bool stringToShapeType(const std::string _shapeTypeString, ImGuiCanvasShapeType& _shapeType);

class ImGuiShape {
private:
  bool visible = true;
public:
  struct CtrlPoint {
    ImVec2 position;
    ImVec2 size = HANDLE_4;
    bool selected = false, hovered = false;
    ImColor color = WHITE;
    // ----------------------------------------------------------------------------------------------------------------
    void setSelected(const bool _selected) {
      selected = _selected;
      color = (_selected)?RED:WHITE;
    }
    bool getSelected() const {
      return selected;
    }
    void select() {
      setSelected(true);
    }
    void deselect() {
      setSelected(false);
    }
    // ----------------------------------------------------------------------------------------------------------------
    void setHovered(const bool _hovered) {
      hovered = _hovered;
      color = (_hovered)?YELLOW:WHITE;
    }
    bool getHovered() const {
      return hovered;
    }
    void hover() {
      setHovered(true);
    }
    // ----------------------------------------------------------------------------------------------------------------
    ImColor getColor() const {
      return color;
    }
    // ----------------------------------------------------------------------------------------------------------------
    void move(const ImVec2& _delta) {
      position.x = position.x+_delta.x;
      position.y = position.y+_delta.y;
    }
    void moveTo(const ImVec2& _position) {
      position = _position;
    }
    // ----------------------------------------------------------------------------------------------------------------
    CtrlPoint(const ImVec2 _position) : position(_position) {}
    CtrlPoint(const float x, const float y) : position({x,y}) {}
    CtrlPoint() = default;
  };
  // ---- constructor -------------------------------------------------------------------------------------------------
  ImGuiShape()=default;
  ImGuiShape(std::string _label, const ImVec2 _center, const ImGuiCanvasShapeType _type, const std::initializer_list<float> _parameters, const ImGuiCanvasClip _clip, const bool _drawCtrls=true);
  ImGuiShape(std::string _label, const ImVec2 _center, const ImGuiCanvasShapeType _type, const ImVector<float> _parameters, const ImGuiCanvasClip _clip, const bool _drawCtrls=true);
  ImGuiShape(std::string _label, const ImVec2 _center, const ImGuiCanvasShapeType _type, const ImVector<ImVec2> _ctrlPoints, const ImGuiCanvasClip _clip, const bool _drawCtrls=true);
  ImGuiShape(const ImGuiShape& _source); // copy constructor

  // ---- destructor --------------------------------------------------------------------------------------------------
  ~ImGuiShape();

  // ---- assignment operator -----------------------------------------------------------------------------------------
  ImGuiShape& operator=(const ImGuiShape& _other);

  // ------------------------------------------------------------------------------------------------------------------
  void setSelected(const bool _selected) {
    m_selected = _selected;
    m_color = (_selected)?RED:WHITE;
  }
  bool getSelected() const {
    return m_selected;
  }
  void select() {
    setSelected(true);
  }
  void deselect() {
    setSelected(false);
  }
  // ------------------------------------------------------------------------------------------------------------------
  void setHovered(const bool _hovered) {
    m_hovered = _hovered;
    m_color = (_hovered)?YELLOW:WHITE;
  }
  bool getHovered() const {
    return m_hovered;
  }
  void hover() {
    setHovered(true);
  }
  // ------------------------------------------------------------------------------------------------------------------
  void setVisible(const bool _visible = true) {
    visible = _visible;
  }
  bool getVisible() const {
    return visible;
  }
  // ------------------------------------------------------------------------------------------------------------------
  CtrlPoint getCenter() const {
    return m_center;
  }
  CtrlPoint& getRefCenter() {
    return m_center;
  }
  std::string getLabel() const {
    return m_label;
  }
  bool isLabel(const std::string _label) const {
    return (m_label == _label);
  }
  uint getIndex() const {
    return m_index;
  }
  ImVector<CtrlPoint> getCtrlPoints() const {
    return m_ctrlPoints;
  }
  CtrlPoint getCtrlPoint(uint _index=0) const {
    return m_ctrlPoints[_index];
  }
  CtrlPoint& getRefCtrlPoint(uint _index=0) {
    return m_ctrlPoints[_index];
  }
  ImGuiCanvasShapeType getType() const {
    return m_type;
  }
  ImColor getColor() const {
    return m_color;
  }
  ImGuiCanvasClip getClip() const {
    return m_clip;
  }
  std::string toString() const;
  float getAngle(uint _index=0) const;
  ImVec2 getRadius(uint _index=0) const;
  float getAbsRadius(uint _index=0) const;

  void setCtrlPoint(CtrlPoint _ctrlPoint, uint _index=0) {
    m_ctrlPoints[_index] = _ctrlPoint;
  }

  bool getCtrlPointSelected(uint _index) const {
    return m_ctrlPoints[_index].getSelected();
  }
  void setCtrlPointSelected(uint _index, const bool _selected) {
    m_ctrlPoints[_index].setSelected(_selected);
  }
  void selectCtrlPoint(uint _index) {
    m_ctrlPoints[_index].select();
  }
  void deselectCtrlPoint(uint _index) {
    m_ctrlPoints[_index].deselect();
  }

  bool getCtrlPointHovered(uint _index) const {
    return m_ctrlPoints[_index].getHovered();
  }
  void setCtrlPointHovered(uint _index, const bool _hovered) {
    m_ctrlPoints[_index].setHovered(_hovered);
  }
  void hoverCtrlPoint(uint _index=0) {
    m_ctrlPoints[_index].hover();
  }
  void setDrawCtrls(const bool _drawCtrls) {
    m_drawCtrls = _drawCtrls;
  }
  bool getDrawCtrls() const {
    return m_drawCtrls;
  }
  inline bool applyClip(float a, float b) const {
    return (bool)m_clip?(a>b):(a<b);
  }
  inline bool applyClip(float a1, float b1, float a2, float b2) const {
    return (bool)m_clip?((a1 < b1)&&(a2 < b2)):((a1 > b1)||(a2 > b2));
  }
  void MoveCenter(const ImVec2& _delta);
  void MoveCenter(const ImVec2& _delta, const ImVec2& _canvasSize);
  void MoveCtrlPoint(int _ctrlPointIndex, const ImVec2& _delta);
  void MoveCtrlPoint(int _ctrlPointIndex, const ImVec2& _delta, const ImVec2& _canvasSize);
  bool isInside(const ImVec2& point) const;
private:
  std::string m_label;
  uint m_index;
  ImGuiCanvasShapeType m_type;
public:
  CtrlPoint m_center;
  ImVector<CtrlPoint> m_ctrlPoints;
private:
  ImGuiCanvasClip m_clip=ImGuiCanvasClip::In;
  bool m_drawCtrls = true;
  static uint nextIndex;
  ImColor m_color = WHITE;
  bool m_selected=false, m_hovered=false;
};
// ====================================================================================================================




















// ====================================================================================================================
namespace ImGui
{
  float length(const ImVec2& a);
  float length(const ImVec2& a, const ImVec2& b);
  float angle(const ImVec2& a);
  float angle(const ImVec2& a, const ImVec2& b);
  bool DrawShapes(const char* _label, const ImVec2& _origin, std::vector<ImGuiShape>& _shapes, const float _scale=1.0, const ImVec2& _size=ImVec2(0,0));
  void UpdateMask(uint8_t* _mask, const ImVec2& _canvasSize, std::vector<ImGuiShape>& _shapes);

  IMGUI_API int DrawCanvas(const char* _label, const ImVec2& _viewSize, const ImVec2& _canvasSize, std::vector<ImGuiShape>& _shapes, const ImColor& _borderColor=WHITE, const ImColor& _bgColor=ImColor(50,50,50,255), ImU32 _flags=0);
  IMGUI_API int DrawCanvas(const char* _label, const ImVec2& _viewSize, const ImVec2& _canvasSize, std::vector<ImGuiShape>& _shapes, ImTextureID _textureId, const ImColor& _borderColor=WHITE, const ImColor& _bgColor=ImColor(50,50,50,255), ImU32 _flags=0);
  IMGUI_API int DrawCanvas(const char* _label, const ImVec2& _viewSize, const ImVec2& _canvasSize, std::vector<ImGuiShape>& _shapes, ImTextureID _textureId, uint8_t* mask, const ImColor& _borderColor=WHITE, const ImColor& _bgColor=ImColor(50,50,50,255), ImU32 _flags=0);
  IMGUI_API int DrawCanvas(const char* _label, const ImVec2& _viewSize, const ImVec2& _canvasSize, ImTextureID _textureId, const ImColor& _borderColor=WHITE, const ImColor& _bgColor=ImColor(50,50,50,255), ImU32 _flags=0);

  IMGUI_API void AddCircle(ImDrawList* _ptrDrawList, const ImVec2& _center, float _radius, const ImColor& _color=WHITE, int _numSegments=32, float _thickness=1.0f);
  IMGUI_API void AddEllipse(ImDrawList* _ptrDrawList, const ImVec2& _center, float _radiusA, float _radiusB, float _tilt, const ImColor& _color=WHITE, int _numSegments=64, float _thickness=1.0f);
  IMGUI_API void AddSquare(ImDrawList* _ptrDrawList, const ImVec2& _center, float _side, float _tilt, const ImColor& _color=WHITE, float _thickness=1.0f);
  IMGUI_API void AddRectangle(ImDrawList* _ptrDrawList, const ImVec2& _center, const ImVec2& _sideA, const ImVec2& _sideB, const ImColor& _color=WHITE, float _thickness=1.0f);
  IMGUI_API void AddHLine(ImDrawList* _ptrDrawList, const ImVec2& _point0, const ImVec2& _point1, const ImColor& _color=WHITE, float _thickness=1.0f);
  IMGUI_API void AddVLine(ImDrawList* _ptrDrawList, const ImVec2& _point0, const ImVec2& _point1, const ImColor& _color=WHITE, float _thickness=1.0f);
}
// ====================================================================================================================
