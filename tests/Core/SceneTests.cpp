#include <entt/entity/entity.hpp>
#include <gtest/gtest.h>
#include <stdexcept>
#include <type_traits>

#include "ProjectLight/Scene/Entity.h"
#include "ProjectLight/Scene/Scene.h"

namespace ProjectLight
{
    static_assert(!std::is_copy_constructible_v<Scene>);
    static_assert(!std::is_copy_assignable_v<Scene>);
    static_assert(!std::is_move_constructible_v<Scene>);
    static_assert(!std::is_move_assignable_v<Scene>);

    TEST(SceneTests, DefaultEntityIsInvalid) {
        const Entity entity;

        EXPECT_FALSE(entity.IsValid());
        EXPECT_FALSE(static_cast<bool>(entity));
        EXPECT_EQ(entity.GetHandle(), entt::null);
    }

    TEST(SceneTests, CreatedEntityIsValidInOwningScene) {
        Scene scene;
        const Entity entity = scene.CreateEntity();

        EXPECT_TRUE(entity.IsValid());
        EXPECT_TRUE(static_cast<bool>(entity));
        EXPECT_TRUE(scene.IsValid(entity));
        EXPECT_NE(entity.GetHandle(), entt::null);
    }

    TEST(SceneTests, CreatedEntitiesHaveDistinctHandlesWithinScene) {
        Scene scene;
        const Entity first = scene.CreateEntity();
        const Entity second = scene.CreateEntity();

        EXPECT_NE(first, second);
        EXPECT_NE(first.GetHandle(), second.GetHandle());
    }

    TEST(SceneTests, EntityIsInvalidForDifferentScene) {
        Scene owningScene;
        Scene otherScene;
        const Entity entity = owningScene.CreateEntity();

        EXPECT_TRUE(owningScene.IsValid(entity));
        EXPECT_FALSE(otherScene.IsValid(entity));
    }

    TEST(SceneTests, EqualRuntimeHandlesFromDifferentScenesAreDifferentEntities) {
        Scene firstScene;
        Scene secondScene;
        const Entity first = firstScene.CreateEntity();
        const Entity second = secondScene.CreateEntity();

        ASSERT_EQ(first.GetHandle(), second.GetHandle());
        EXPECT_NE(first, second);
    }

    TEST(SceneTests, DestroyingEntityInvalidatesEveryWrapperCopy) {
        Scene scene;
        const Entity entity = scene.CreateEntity();
        const Entity copy = entity;

        ASSERT_TRUE(scene.DestroyEntity(entity));

        EXPECT_FALSE(entity.IsValid());
        EXPECT_FALSE(copy.IsValid());
        EXPECT_FALSE(scene.IsValid(entity));
    }

    TEST(SceneTests, DestroyingInvalidEntityReturnsFalse) {
        Scene scene;
        const Entity entity = scene.CreateEntity();

        ASSERT_TRUE(scene.DestroyEntity(entity));
        EXPECT_FALSE(scene.DestroyEntity(entity));
        EXPECT_FALSE(scene.DestroyEntity(Entity{}));
    }

    TEST(SceneTests, ForeignEntityCannotDestroyMatchingHandle) {
        Scene firstScene;
        Scene secondScene;
        const Entity first = firstScene.CreateEntity();
        const Entity second = secondScene.CreateEntity();

        ASSERT_EQ(first.GetHandle(), second.GetHandle());
        EXPECT_FALSE(firstScene.DestroyEntity(second));
        EXPECT_TRUE(first.IsValid());
        EXPECT_TRUE(second.IsValid());
    }

    TEST(SceneTests, EntityUsesDefaultNameWhenNoneIsProvided) {
        Scene scene;
        const Entity entity = scene.CreateEntity();

        EXPECT_EQ(scene.GetEntityName(entity), "Entity");
    }

    TEST(SceneTests, EntityStoresCustomNameAtCreation) {
        Scene scene;
        const Entity entity = scene.CreateEntity("Player");

        EXPECT_EQ(scene.GetEntityName(entity), "Player");
    }

    TEST(SceneTests, EntityCanBeRenamed) {
        Scene scene;
        const Entity entity = scene.CreateEntity("Before");

        scene.SetEntityName(entity, "After");

        EXPECT_EQ(scene.GetEntityName(entity), "After");
    }

    TEST(SceneTests, NameAccessRejectsInvalidEntity) {
        Scene scene;
        const Entity entity = scene.CreateEntity();
        ASSERT_TRUE(scene.DestroyEntity(entity));

        EXPECT_THROW(static_cast<void>(scene.GetEntityName(entity)), std::invalid_argument);
        EXPECT_THROW(scene.SetEntityName(entity, "Invalid"), std::invalid_argument);
        EXPECT_THROW(static_cast<void>(scene.GetEntityName(Entity{})), std::invalid_argument);
    }

    TEST(SceneTests, EntityReceivesDefaultTransform) {
        Scene scene;
        const Entity entity = scene.CreateEntity();
        const TransformComponent& transform = scene.GetEntityTransform(entity);

        EXPECT_FLOAT_EQ(transform.Translation.x, 0.0F);
        EXPECT_FLOAT_EQ(transform.Translation.y, 0.0F);
        EXPECT_FLOAT_EQ(transform.Translation.z, 0.0F);
        EXPECT_FLOAT_EQ(transform.Rotation.x, 0.0F);
        EXPECT_FLOAT_EQ(transform.Rotation.y, 0.0F);
        EXPECT_FLOAT_EQ(transform.Rotation.z, 0.0F);
        EXPECT_FLOAT_EQ(transform.Scale.x, 1.0F);
        EXPECT_FLOAT_EQ(transform.Scale.y, 1.0F);
        EXPECT_FLOAT_EQ(transform.Scale.z, 1.0F);
    }

    TEST(SceneTests, MutableTransformChangesAreVisibleThroughConstScene) {
        Scene scene;
        const Entity entity = scene.CreateEntity();

        TransformComponent& transform = scene.GetEntityTransform(entity);
        transform.Translation = {1.0F, 2.0F, 3.0F};
        transform.Rotation = {10.0F, 20.0F, 30.0F};
        transform.Scale = {2.0F, 3.0F, 4.0F};

        const Scene& readOnlyScene = scene;
        const TransformComponent& storedTransform = readOnlyScene.GetEntityTransform(entity);

        EXPECT_FLOAT_EQ(storedTransform.Translation.x, 1.0F);
        EXPECT_FLOAT_EQ(storedTransform.Translation.y, 2.0F);
        EXPECT_FLOAT_EQ(storedTransform.Translation.z, 3.0F);
        EXPECT_FLOAT_EQ(storedTransform.Rotation.x, 10.0F);
        EXPECT_FLOAT_EQ(storedTransform.Rotation.y, 20.0F);
        EXPECT_FLOAT_EQ(storedTransform.Rotation.z, 30.0F);
        EXPECT_FLOAT_EQ(storedTransform.Scale.x, 2.0F);
        EXPECT_FLOAT_EQ(storedTransform.Scale.y, 3.0F);
        EXPECT_FLOAT_EQ(storedTransform.Scale.z, 4.0F);
    }

    TEST(SceneTests, TransformAccessRejectsInvalidEntity) {
        Scene scene;
        const Entity entity = scene.CreateEntity();
        ASSERT_TRUE(scene.DestroyEntity(entity));

        EXPECT_THROW(static_cast<void>(scene.GetEntityTransform(entity)), std::invalid_argument);
        EXPECT_THROW(static_cast<void>(scene.GetEntityTransform(Entity{})), std::invalid_argument);
    }

    TEST(SceneTests, EntityReceivesStableValidUUID) {
        Scene scene;
        const Entity entity = scene.CreateEntity();

        const UUID firstRead = scene.GetEntityUUID(entity);
        const UUID secondRead = scene.GetEntityUUID(entity);

        EXPECT_TRUE(firstRead.IsValid());
        EXPECT_EQ(firstRead, secondRead);
    }

    TEST(SceneTests, DifferentEntitiesReceiveDifferentUUIDs) {
        Scene scene;
        const Entity first = scene.CreateEntity();
        const Entity second = scene.CreateEntity();

        EXPECT_NE(scene.GetEntityUUID(first), scene.GetEntityUUID(second));
    }

    TEST(SceneTests, UUIDAccessRejectsInvalidEntity) {
        Scene scene;
        const Entity entity = scene.CreateEntity();
        ASSERT_TRUE(scene.DestroyEntity(entity));

        EXPECT_THROW(static_cast<void>(scene.GetEntityUUID(entity)), std::invalid_argument);
        EXPECT_THROW(static_cast<void>(scene.GetEntityUUID(Entity{})), std::invalid_argument);
    }

    TEST(SceneTests, NewEntityStartsAsRootWithoutChildren) {
        Scene scene;
        const Entity entity = scene.CreateEntity();

        EXPECT_FALSE(scene.GetParent(entity).IsValid());
        EXPECT_EQ(scene.GetChildCount(entity), 0);
        EXPECT_THROW(static_cast<void>(scene.GetChild(entity, 0)), std::out_of_range);
    }

    TEST(SceneTests, SettingParentUpdatesBothHierarchyDirections) {
        Scene scene;
        const Entity parent = scene.CreateEntity("Parent");
        const Entity child = scene.CreateEntity("Child");

        ASSERT_TRUE(scene.SetParent(child, parent));

        EXPECT_EQ(scene.GetParent(child), parent);
        ASSERT_EQ(scene.GetChildCount(parent), 1);
        EXPECT_EQ(scene.GetChild(parent, 0), child);
    }

    TEST(SceneTests, SettingSameParentTwiceDoesNotDuplicateChild) {
        Scene scene;
        const Entity parent = scene.CreateEntity();
        const Entity child = scene.CreateEntity();

        ASSERT_TRUE(scene.SetParent(child, parent));
        ASSERT_TRUE(scene.SetParent(child, parent));

        EXPECT_EQ(scene.GetChildCount(parent), 1);
    }

    TEST(SceneTests, ReparentingUpdatesPreviousAndNewParent) {
        Scene scene;
        const Entity previousParent = scene.CreateEntity();
        const Entity newParent = scene.CreateEntity();
        const Entity child = scene.CreateEntity();

        ASSERT_TRUE(scene.SetParent(child, previousParent));
        ASSERT_TRUE(scene.SetParent(child, newParent));

        EXPECT_EQ(scene.GetChildCount(previousParent), 0);
        ASSERT_EQ(scene.GetChildCount(newParent), 1);
        EXPECT_EQ(scene.GetChild(newParent, 0), child);
        EXPECT_EQ(scene.GetParent(child), newParent);
    }

    TEST(SceneTests, ClearingParentMakesEntityRoot) {
        Scene scene;
        const Entity parent = scene.CreateEntity();
        const Entity child = scene.CreateEntity();

        ASSERT_TRUE(scene.SetParent(child, parent));
        ASSERT_TRUE(scene.ClearParent(child));
        ASSERT_TRUE(scene.ClearParent(child));

        EXPECT_FALSE(scene.GetParent(child).IsValid());
        EXPECT_EQ(scene.GetChildCount(parent), 0);
    }

    TEST(SceneTests, ParentingRejectsSelfInvalidAndForeignEntities) {
        Scene scene;
        Scene otherScene;
        const Entity entity = scene.CreateEntity();
        const Entity other = otherScene.CreateEntity();

        EXPECT_FALSE(scene.SetParent(entity, entity));
        EXPECT_FALSE(scene.SetParent(entity, Entity{}));
        EXPECT_FALSE(scene.SetParent(Entity{}, entity));
        EXPECT_FALSE(scene.SetParent(entity, other));
        EXPECT_FALSE(scene.ClearParent(Entity{}));
        EXPECT_FALSE(scene.GetParent(entity).IsValid());
    }

    TEST(SceneTests, ParentingRejectsCycles) {
        Scene scene;
        const Entity root = scene.CreateEntity("Root");
        const Entity child = scene.CreateEntity("Child");
        const Entity grandchild = scene.CreateEntity("Grandchild");

        ASSERT_TRUE(scene.SetParent(child, root));
        ASSERT_TRUE(scene.SetParent(grandchild, child));

        EXPECT_FALSE(scene.SetParent(root, grandchild));
        EXPECT_FALSE(scene.GetParent(root).IsValid());
        EXPECT_EQ(scene.GetParent(child), root);
        EXPECT_EQ(scene.GetParent(grandchild), child);
    }

    TEST(SceneTests, DestroyingChildRemovesItWithoutReorderingRemainingChildren) {
        Scene scene;
        const Entity parent = scene.CreateEntity();
        const Entity first = scene.CreateEntity();
        const Entity second = scene.CreateEntity();

        ASSERT_TRUE(scene.SetParent(first, parent));
        ASSERT_TRUE(scene.SetParent(second, parent));
        ASSERT_TRUE(scene.DestroyEntity(first));

        ASSERT_EQ(scene.GetChildCount(parent), 1);
        EXPECT_EQ(scene.GetChild(parent, 0), second);
        EXPECT_TRUE(second.IsValid());
    }

    TEST(SceneTests, DestroyingParentRecursivelyInvalidatesDescendants) {
        Scene scene;
        const Entity root = scene.CreateEntity();
        const Entity child = scene.CreateEntity();
        const Entity grandchild = scene.CreateEntity();

        ASSERT_TRUE(scene.SetParent(child, root));
        ASSERT_TRUE(scene.SetParent(grandchild, child));
        ASSERT_TRUE(scene.DestroyEntity(root));

        EXPECT_FALSE(root.IsValid());
        EXPECT_FALSE(child.IsValid());
        EXPECT_FALSE(grandchild.IsValid());
    }

    TEST(SceneTests, HierarchyQueriesRejectInvalidEntities) {
        Scene scene;
        const Entity invalid;

        EXPECT_THROW(static_cast<void>(scene.GetParent(invalid)), std::invalid_argument);
        EXPECT_THROW(static_cast<void>(scene.GetChildCount(invalid)), std::invalid_argument);
        EXPECT_THROW(static_cast<void>(scene.GetChild(invalid, 0)), std::invalid_argument);
    }
}
