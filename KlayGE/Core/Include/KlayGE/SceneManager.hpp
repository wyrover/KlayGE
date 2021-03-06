// SceneManager.hpp
// KlayGE 场景管理器类 头文件
// Ver 3.12.0
// 版权所有(C) 龚敏敏, 2003-2011
// Homepage: http://www.klayge.org
//
// 3.11.0
// 把DoAddSceneObject/DoDelSceneObject改为OnAddSceneObject/OnDelSceneObject (2010.11.16)
//
// 3.9.0
// 增加了SceneObjects (2009.7.30)
//
// 2.4.0
// 增加了NumObjectsRendered，NumPrimitivesRendered和NumVerticesRendered (2005.3.20)
//
// 修改记录
//////////////////////////////////////////////////////////////////////////////////

#ifndef _SCENEMANAGER_HPP
#define _SCENEMANAGER_HPP

#pragma once

#include <KlayGE/PreDeclare.hpp>

#include <KlayGE/Renderable.hpp>
#include <KFL/Frustum.hpp>
#include <KFL/Thread.hpp>

#include <vector>

#include <boost/noncopyable.hpp>

namespace KlayGE
{
	class KLAYGE_CORE_API SceneManager : boost::noncopyable
	{
	protected:
		typedef std::vector<SceneObjectPtr> SceneObjsType;
		typedef std::vector<RenderablePtr> RenderItemsType;
		typedef std::vector<std::pair<RenderTechniquePtr, RenderItemsType> > RenderQueueType;

	public:
		SceneManager();
		virtual ~SceneManager();

		void Suspend();
		void Resume();

		void SmallObjectThreshold(float area);
		void SceneUpdateElapse(float elapse);
		virtual void ClipScene();

		void AddCamera(CameraPtr const & camera);
		void DelCamera(CameraPtr const & camera);

		uint32_t NumCameras() const;
		CameraPtr& GetCamera(uint32_t index);
		CameraPtr const & GetCamera(uint32_t index) const;

		void AddLight(LightSourcePtr const & light);
		void DelLight(LightSourcePtr const & light);

		uint32_t NumLights() const;
		LightSourcePtr& GetLight(uint32_t index);
		LightSourcePtr const & GetLight(uint32_t index) const;

		void AddSceneObject(SceneObjectPtr const & obj);
		void AddSceneObjectLocked(SceneObjectPtr const & obj);
		void DelSceneObject(SceneObjectPtr const & obj);
		void DelSceneObjectLocked(SceneObjectPtr const & obj);
		void AddRenderable(RenderablePtr const & obj);

		uint32_t NumSceneObjects() const;
		SceneObjectPtr& GetSceneObject(uint32_t index);
		SceneObjectPtr const & GetSceneObject(uint32_t index) const;

		virtual BoundOverlap AABBVisible(AABBox const & aabb) const;
		virtual BoundOverlap OBBVisible(OBBox const & obb) const;
		virtual BoundOverlap SphereVisible(Sphere const & sphere) const;
		virtual BoundOverlap FrustumVisible(Frustum const & frustum) const;

		virtual void ClearCamera();
		virtual void ClearLight();
		virtual void ClearObject();

		void Update();

		uint32_t NumObjectsRendered() const;
		uint32_t NumRenderablesRendered() const;
		uint32_t NumPrimitivesRendered() const;
		uint32_t NumVerticesRendered() const;
		uint32_t NumDrawCalls() const;
		uint32_t NumDispatchCalls() const;

	protected:
		void Flush(uint32_t urt);

		std::vector<CameraPtr>::iterator DelCamera(std::vector<CameraPtr>::iterator iter);
		std::vector<LightSourcePtr>::iterator DelLight(std::vector<LightSourcePtr>::iterator iter);
		SceneObjsType::iterator DelSceneObject(SceneObjsType::iterator iter);
		SceneObjsType::iterator DelSceneObjectLocked(SceneObjsType::iterator iter);
		virtual void OnAddSceneObject(SceneObjectPtr const & obj) = 0;
		virtual void OnDelSceneObject(SceneObjsType::iterator iter) = 0;
		virtual void DoSuspend() = 0;
		virtual void DoResume() = 0;

		void UpdateThreadFunc();

		BoundOverlap VisibleTestFromParent(SceneObjectPtr const & obj,
			float3 const & eye_pos, float4x4 const & view_proj);

	protected:
		std::vector<CameraPtr> cameras_;
		Frustum const * frustum_;
		std::vector<LightSourcePtr> lights_;
		SceneObjsType scene_objs_;
		SceneObjsType overlay_scene_objs_;

		unordered_map<size_t, shared_ptr<std::vector<BoundOverlap> > > visible_marks_map_;

		float small_obj_threshold_;
		float update_elapse_;

	private:
		void FlushScene();

	private:
		uint32_t urt_;

		RenderQueueType render_queue_;

		uint32_t num_objects_rendered_;
		uint32_t num_renderables_rendered_;
		uint32_t num_primitives_rendered_;
		uint32_t num_vertices_rendered_;
		uint32_t num_draw_calls_;
		uint32_t num_dispatch_calls_;

		mutex update_mutex_;
		shared_ptr<joiner<void> > update_thread_;
		bool quit_;

		bool deferred_mode_;
	};
}

#endif			// _SCENEMANAGER_HPP
