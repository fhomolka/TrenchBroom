/*
 Copyright (C) 2010-2017 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "Model/IssueType.h"

#include <string>
#include <vector>

namespace TrenchBroom {
namespace Model {
class BrushFace;
class BrushNode;
class Node;

class Issue {
private:
  size_t m_seqId;

protected:
  Node* const m_node;

public:
  virtual ~Issue();

  size_t seqId() const;
  size_t lineNumber() const;
  std::string description() const;

  IssueType type() const;
  Node* node() const;

  bool addSelectableNodes(std::vector<Model::Node*>& nodes) const;

  bool hidden() const;
  void setHidden(bool hidden);

protected:
  explicit Issue(Node* node);
  static size_t nextSeqId();
  static IssueType freeType();

private: // subclassing interface
  virtual size_t doGetLineNumber() const;
  virtual IssueType doGetType() const = 0;
  virtual std::string doGetDescription() const = 0;
};

class BrushFaceIssue : public Issue {
private:
  const size_t m_faceIndex;

protected:
  explicit BrushFaceIssue(BrushNode* node, size_t faceIndex);

public:
  ~BrushFaceIssue() override;
  size_t faceIndex() const;
  const BrushFace& face() const;

private:
  size_t doGetLineNumber() const override;
};

class EntityPropertyIssue : public Issue {
public:
  using Issue::Issue;

  ~EntityPropertyIssue() override;
  virtual const std::string& propertyKey() const = 0;
  const std::string& propertyValue() const;
};
} // namespace Model
} // namespace TrenchBroom
