/*
 Copyright (C) 2010-2014 Kristian Duske
 
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

#include "MdlModel.h"

#include "CollectionUtils.h"
#include "Assets/Texture.h"
#include "Assets/Texture.h"
#include "Renderer/TriangleMeshRenderer.h"
#include "Renderer/TriangleMesh.h"
#include "Renderer/VertexSpec.h"

#include <cassert>

namespace TrenchBroom {
    namespace Assets {
        MdlSkin::MdlSkin(Texture* texture) :
        m_textures("", TextureList(1, texture)) {
            m_times.push_back(0.0f);
        }
        
        MdlSkin::MdlSkin(const TextureList& textures, const MdlTimeList& times) :
        m_textures("", textures),
        m_times(times) {
            assert(textures.size() == times.size());
        }
        
        void MdlSkin::prepare(const int minFilter, const int magFilter) {
            m_textures.prepare(minFilter, magFilter);
        }

        const Texture* MdlSkin::firstPicture() const {
            return m_textures.textures().front();
        }

        MdlBaseFrame::~MdlBaseFrame() {}

        MdlFrame::MdlFrame(const String& name, const MdlFrameVertexList& triangles, const BBox3f& bounds) :
        m_name(name),
        m_triangles(triangles),
        m_bounds(bounds) {}
        
        const MdlFrame* MdlFrame::firstFrame() const {
            return this;
        }

        const MdlFrameVertexList& MdlFrame::triangles() const {
            return m_triangles;
        }

        BBox3f MdlFrame::bounds() const {
            return m_bounds;
        }

        BBox3f MdlFrame::transformedBounds(const Mat4x4f& transformation) const {
            if (m_triangles.empty())
                return BBox3f(-8.0f, 8.0f);
            
            MdlFrameVertexList::const_iterator it = m_triangles.begin();
            MdlFrameVertexList::const_iterator end = m_triangles.end();
            
            BBox3f bounds;
            bounds.min = bounds.max = transformation * it->v1;
            ++it;
            
            while (it != end) {
                bounds.mergeWith(transformation * it->v1);
                ++it;
            }
            
            return bounds;
        }

        MdlFrameGroup::~MdlFrameGroup() {
            VectorUtils::clearAndDelete(m_frames);
        }
        
        const MdlFrame* MdlFrameGroup::firstFrame() const {
            if (m_frames.empty())
                return NULL;
            return m_frames[0]->firstFrame();
        }
        
        void MdlFrameGroup::addFrame(MdlFrame* frame, const float time) {
            m_frames.push_back(frame);
            m_times.push_back(time);
        }

        MdlModel::MdlModel(const String& name) :
        m_name(name) {}

        MdlModel::~MdlModel() {
            VectorUtils::clearAndDelete(m_skins);
            VectorUtils::clearAndDelete(m_frames);
        }

        void MdlModel::addSkin(MdlSkin* skin) {
            m_skins.push_back(skin);
        }

        void MdlModel::addFrame(MdlBaseFrame* frame) {
            m_frames.push_back(frame);
        }

        Renderer::TexturedTriangleMeshRenderer* MdlModel::doBuildRenderer(const size_t skinIndex, const size_t frameIndex) const {
            if (skinIndex >= m_skins.size())
                return NULL;
            if (frameIndex >= m_frames.size())
                return NULL;
            const MdlSkin* skin = m_skins[skinIndex];
            const MdlFrame* frame = m_frames[frameIndex]->firstFrame();

            typedef Renderer::TriangleMesh<Renderer::VertexSpecs::P3T2, const Texture*> Mesh;
            
            Mesh::MeshSize size;
            size.addSet(skin->firstPicture(), frame->triangles().size());
            
            Mesh mesh(size);
            mesh.beginTriangleSet(skin->firstPicture());
            mesh.addTrianglesToSet(frame->triangles());
            mesh.endTriangleSet();
            
            return new Renderer::TexturedTriangleMeshRenderer(mesh);
        }

        BBox3f MdlModel::doGetBounds(const size_t skinIndex, const size_t frameIndex) const {
            if (frameIndex >= m_frames.size())
                return BBox3f(-8.0f, 8.0f);
            const MdlFrame* frame = m_frames[frameIndex]->firstFrame();
            return frame->bounds();
        }

        BBox3f MdlModel::doGetTransformedBounds(const size_t skinIndex, const size_t frameIndex, const Mat4x4f& transformation) const {
            if (frameIndex >= m_frames.size())
                return BBox3f(-8.0f, 8.0f);
            const MdlFrame* frame = m_frames[frameIndex]->firstFrame();
            return frame->transformedBounds(transformation);
        }

        
        void MdlModel::doPrepare() {
            for (size_t i = 0; i < m_skins.size(); ++i)
                m_skins[i]->prepare(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        }
    }
}
