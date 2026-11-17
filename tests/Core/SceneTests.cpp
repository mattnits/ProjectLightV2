#include <entt/entity/entity.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <gtest/gtest.h>
#include <stdexcept>
#include <type_traits>

#include "ProjectLight/Scene/Components/MeshRendererComponent.h"
#include "ProjectLight/Scene/Entity.h"
#include "ProjectLight/Scene/Scene.h"

namespace ProjectLight
{
    namespace
    {
        constexpr float MatrixTolerance = 0.0001F;

        void ExpectMatrixNear(const glm::mat4& actual, const glm::mat4& expected) {
            for (int column = 0; column < 4; ++column) {
                for (int row = 0; row < 4; ++row) {
                    EXPECT_NEAR(actual[column][row], expected[column][row], MatrixTolerance);
                }
            }
        }

        void ExpectTransformedOrigin(const glm::mat4& matrix, const glm::vec3& expected) {
            const glm::vec4 actual = matrix * glm::vec4{0.0F, 0.0F, 0.0F, 1.0F};

            EXPECT_NEAR(actual.x, expected.x, MatrixTolerance);
            EXPECT_NEAR(actual.y, expected.y, MatrixTolerance);
            EXPECT_NEAR(actual.z, expected.z, MatrixTolerance);
            EXPECT_NEAR(actual.w, 1.0F, MatrixTolerance);
        }
    }

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

    TEST(SceneTests, RootWorldMatrixEqualsLocalMatrix) {
        Scene scene;
        const Entity entity = scene.CreateEntity();
        TransformComponent& transform = scene.GetEntityTransform(entity);
        transform.Translation = {1.0F, 2.0F, 3.0F};
        transform.Rotation = {10.0F, 20.0F, 30.0F};
        transform.Scale = {2.0F, 3.0F, 4.0F};

        ExpectMatrixNear(scene.GetWorldTransformMatrix(entity),
                         scene.GetLocalTransformMatrix(entity));
    }

    TEST(SceneTests, WorldMatrixCombinesMultiLevelTranslations) {
        Scene scene;
        const Entity root = scene.CreateEntity();
        const Entity child = scene.CreateEntity();
        const Entity grandchild = scene.CreateEntity();

        scene.GetEntityTransform(root).Translation = {10.0F, 0.0F, 0.0F};
        scene.GetEntityTransform(child).Translation = {0.0F, 2.0F, 0.0F};
        scene.GetEntityTransform(grandchild).Translation = {0.0F, 0.0F, 3.0F};
        ASSERT_TRUE(scene.SetParent(child, root));
        ASSERT_TRUE(scene.SetParent(grandchild, child));

        ExpectTransformedOrigin(scene.GetWorldTransformMatrix(grandchild),
                                glm::vec3{10.0F, 2.0F, 3.0F});
    }

    TEST(SceneTests, ChildWorldMatrixInheritsParentRotation) {
        Scene scene;
        const Entity parent = scene.CreateEntity();
        const Entity child = scene.CreateEntity();

        scene.GetEntityTransform(parent).Rotation = {0.0F, 0.0F, 90.0F};
        scene.GetEntityTransform(child).Translation = {2.0F, 0.0F, 0.0F};
        ASSERT_TRUE(scene.SetParent(child, parent));

        ExpectTransformedOrigin(scene.GetWorldTransformMatrix(child), glm::vec3{0.0F, 2.0F, 0.0F});
    }

    TEST(SceneTests, ReparentingChangesWorldMatrixButPreservesLocalMatrix) {
        Scene scene;
        const Entity firstParent = scene.CreateEntity();
        const Entity secondParent = scene.CreateEntity();
        const Entity child = scene.CreateEntity();

        scene.GetEntityTransform(firstParent).Translation = {5.0F, 0.0F, 0.0F};
        scene.GetEntityTransform(secondParent).Translation = {-5.0F, 0.0F, 0.0F};
        scene.GetEntityTransform(child).Translation = {1.0F, 0.0F, 0.0F};

        const glm::mat4 originalLocalMatrix = scene.GetLocalTransformMatrix(child);
        ASSERT_TRUE(scene.SetParent(child, firstParent));
        ExpectTransformedOrigin(scene.GetWorldTransformMatrix(child), glm::vec3{6.0F, 0.0F, 0.0F});

        ASSERT_TRUE(scene.SetParent(child, secondParent));
        ExpectTransformedOrigin(scene.GetWorldTransformMatrix(child), glm::vec3{-4.0F, 0.0F, 0.0F});
        ExpectMatrixNear(scene.GetLocalTransformMatrix(child), originalLocalMatrix);
    }

    TEST(SceneTests, TransformMatrixQueriesRejectInvalidEntity) {
        Scene scene;
        const Entity invalid;

        EXPECT_THROW(static_cast<void>(scene.GetLocalTransformMatrix(invalid)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(scene.GetWorldTransformMatrix(invalid)),
                     std::invalid_argument);
    }

    TEST(SceneTests, RequiredComponentsAreAvailableThroughGenericAccess) {
        Scene scene;
        const Entity entity = scene.CreateEntity();

        EXPECT_TRUE(scene.HasComponent<TransformComponent>(entity));
        EXPECT_EQ(&scene.GetComponent<TransformComponent>(entity),
                  &scene.GetEntityTransform(entity));
    }

    TEST(SceneTests, OptionalComponentCanBeAddedAndRetrieved) {
        Scene scene;
        const Entity entity = scene.CreateEntity();

        EXPECT_FALSE(scene.HasComponent<MeshRendererComponent>(entity));

        MeshRendererComponent& component = scene.AddComponent<MeshRendererComponent>(entity);
        component.IsVisible = false;

        EXPECT_TRUE(scene.HasComponent<MeshRendererComponent>(entity));
        EXPECT_EQ(component.MeshResource, nullptr);
        EXPECT_FALSE(scene.GetComponent<MeshRendererComponent>(entity).IsVisible);

        const Scene& readOnlyScene = scene;
        EXPECT_FALSE(readOnlyScene.GetComponent<MeshRendererComponent>(entity).IsVisible);
    }

    TEST(SceneTests, AddingDuplicateComponentIsRejected) {
        Scene scene;
        const Entity entity = scene.CreateEntity();
        static_cast<void>(scene.AddComponent<MeshRendererComponent>(entity));

        EXPECT_THROW(static_cast<void>(scene.AddComponent<MeshRendererComponent>(entity)),
                     std::logic_error);
    }

    TEST(SceneTests, OptionalComponentCanBeRemoved) {
        Scene scene;
        const Entity entity = scene.CreateEntity();
        static_cast<void>(scene.AddComponent<MeshRendererComponent>(entity));

        EXPECT_TRUE(scene.RemoveComponent<MeshRendererComponent>(entity));
        EXPECT_FALSE(scene.HasComponent<MeshRendererComponent>(entity));
        EXPECT_FALSE(scene.RemoveComponent<MeshRendererComponent>(entity));
        EXPECT_THROW(static_cast<void>(scene.GetComponent<MeshRendererComponent>(entity)),
                     std::out_of_range);
    }

    TEST(SceneTests, GenericComponentOperationsRejectInvalidEntity) {
        Scene scene;
        const Entity invalid;

        EXPECT_FALSE(scene.HasComponent<MeshRendererComponent>(invalid));
        EXPECT_FALSE(scene.RemoveComponent<MeshRendererComponent>(invalid));
        EXPECT_THROW(static_cast<void>(scene.AddComponent<MeshRendererComponent>(invalid)),
                     std::invalid_argument);
        EXPECT_THROW(static_cast<void>(scene.GetComponent<MeshRendererComponent>(invalid)),
                     std::invalid_argument);
    }

    TEST(SceneTests, DuplicateGetsNewIdentityAndCopiesNameAndTransform) {
        Scene scene;
        const Entity source = scene.CreateEntity("Source");
        TransformComponent& sourceTransform = scene.GetEntityTransform(source);
        sourceTransform.Translation = {1.0F, 2.0F, 3.0F};
        sourceTransform.Rotation = {10.0F, 20.0F, 30.0F};
        sourceTransform.Scale = {2.0F, 3.0F, 4.0F};

        const Entity duplicate = scene.DuplicateEntity(source);
        const TransformComponent& duplicateTransform = scene.GetEntityTransform(duplicate);

        EXPECT_NE(duplicate, source);
        EXPECT_NE(scene.GetEntityUUID(duplicate), scene.GetEntityUUID(source));
        EXPECT_EQ(scene.GetEntityName(duplicate), "Source Copy");
        EXPECT_FLOAT_EQ(duplicateTransform.Translation.x, 1.0F);
        EXPECT_FLOAT_EQ(duplicateTransform.Translation.y, 2.0F);
        EXPECT_FLOAT_EQ(duplicateTransform.Translation.z, 3.0F);
        EXPECT_FLOAT_EQ(duplicateTransform.Rotation.x, 10.0F);
        EXPECT_FLOAT_EQ(duplicateTransform.Rotation.y, 20.0F);
        EXPECT_FLOAT_EQ(duplicateTransform.Rotation.z, 30.0F);
        EXPECT_FLOAT_EQ(duplicateTransform.Scale.x, 2.0F);
        EXPECT_FLOAT_EQ(duplicateTransform.Scale.y, 3.0F);
        EXPECT_FLOAT_EQ(duplicateTransform.Scale.z, 4.0F);
    }

    TEST(SceneTests, DuplicateCopiesOptionalMeshRendererSettings) {
        Scene scene;
        const Entity source = scene.CreateEntity();
        MeshRendererComponent& sourceRenderer = scene.AddComponent<MeshRendererComponent>(source);
        sourceRenderer.IsVisible = false;

        const Entity duplicate = scene.DuplicateEntity(source);

        ASSERT_TRUE(scene.HasComponent<MeshRendererComponent>(duplicate));
        const MeshRendererComponent& duplicateRenderer =
            scene.GetComponent<MeshRendererComponent>(duplicate);
        EXPECT_EQ(duplicateRenderer.MeshResource, sourceRenderer.MeshResource);
        EXPECT_EQ(duplicateRenderer.IsVisible, sourceRenderer.IsVisible);
    }

    TEST(SceneTests, DuplicateStartsAsRootWithoutCopyingChildren) {
        Scene scene;
        const Entity parent = scene.CreateEntity();
        const Entity source = scene.CreateEntity();
        const Entity child = scene.CreateEntity();
        ASSERT_TRUE(scene.SetParent(source, parent));
        ASSERT_TRUE(scene.SetParent(child, source));

        const Entity duplicate = scene.DuplicateEntity(source);

        EXPECT_FALSE(scene.GetParent(duplicate).IsValid());
        EXPECT_EQ(scene.GetChildCount(duplicate), 0);
        EXPECT_EQ(scene.GetParent(source), parent);
        ASSERT_EQ(scene.GetChildCount(source), 1);
        EXPECT_EQ(scene.GetChild(source, 0), child);
    }

    TEST(SceneTests, DuplicateRejectsInvalidEntity) {
        Scene scene;
        const Entity destroyed = scene.CreateEntity();
        ASSERT_TRUE(scene.DestroyEntity(destroyed));

        EXPECT_THROW(static_cast<void>(scene.DuplicateEntity(Entity{})), std::invalid_argument);
        EXPECT_THROW(static_cast<void>(scene.DuplicateEntity(destroyed)), std::invalid_argument);
    }

    TEST(SceneTests, RenderIterationSkipsEntitiesWithoutUsableMesh) {
        Scene scene;
        static_cast<void>(scene.CreateEntity("No renderer"));

        const Entity unassignedMesh = scene.CreateEntity("Unassigned mesh");
        static_cast<void>(scene.AddComponent<MeshRendererComponent>(unassignedMesh));

        int callbackCount = 0;
        scene.ForEachRenderable([&](const auto&, const glm::mat4&) { ++callbackCount; });

        EXPECT_EQ(callbackCount, 0);
    }
}
