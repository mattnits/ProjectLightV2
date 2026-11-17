# ECS Architecture

## Ownership

```text
Editor or runtime
└── owns Scene
    └── owns one entt::registry
        ├── owns entities
        └── owns component storage

Entity
├── holds a non-owning Scene pointer
└── holds a temporary entt::entity handle
```

- Each `Scene` owns exactly one `entt::registry` by value.
- No global registry exists.
- `Entity` does not own the scene, registry, entity, or components.
- Destroying an `Entity` wrapper does not destroy its registry entity.
- The `Scene` controls entity creation and destruction.
- A wrapper is valid only when its scene exists and its handle remains valid in that scene.

## Identity

- `entt::entity` is a temporary runtime handle.
- Runtime handles may be reused after entity destruction.
- Runtime handles must not be saved, serialized, or used as persistent asset references.
- A UUID component will provide persistent identity.
- Cross-scene and serialized references will use UUIDs rather than `entt::entity`.

## Components

- Components are primarily plain data.
- The registry owns component storage.
- Components do not own their entity or scene.
- Scene operations enforce lifecycle and relationship rules.
- Engine systems query the registry through focused `Scene` operations.

### Required components

- Every scene-created entity receives a `TagComponent` and `TransformComponent`.
- Required components are created atomically; a failed component construction rolls back the entity.
- ProjectLight does not expose removal of required components.
- Transform translation, rotation, and scale are local to the entity's future parent.
- Rotation values are Euler angles stored in degrees for editor usability.
- Default translation and rotation are zero; default scale is one.
- Local/world matrix composition and hierarchy propagation are defined in later Phase 5 steps.

### Hierarchy rules

- Every entity has a required `RelationshipComponent`; a null parent identifies a root.
- Parent and child must belong to the same scene, and an entity cannot parent itself.
- Reparenting walks the proposed parent chain and rejects cycles.
- Reparenting removes the child from its previous parent and appends it once to the new parent.
- Child order is stable and follows parenting order for future hierarchy-panel traversal.
- Reparenting currently preserves the local transform rather than the world transform.
- Destroying an entity recursively destroys all descendants and removes it from its own parent.
- Runtime links use `entt::entity`; serialization will translate relationships through persistent UUIDs.

## Encapsulation

- The registry remains private to `Scene`.
- `Entity` receives only the access required for component operations.
- Editor and runtime code should not depend directly on registry storage details.
- EnTT provides storage and queries; ProjectLight defines scene semantics.

## Lifetime rules

- A `Scene` must outlive every `Entity` wrapper referring to it.
- Destroying a scene invalidates all wrappers for that scene.
- Destroying a registry entity invalidates every wrapper containing that handle.
- Entity validity must be checked before registry access.
- An entity handle from one scene must never be used with another scene's registry.

## Application ownership

- `EditorApplication` exclusively owns its active scene with `std::unique_ptr<Scene>`.
- A runtime application will likewise own its runtime scene; `Engine` does not globally own project scenes.
- Heap ownership gives the scene a stable address while it is active, even if its owning pointer moves.
- Scene replacement destroys the old scene and deliberately invalidates all wrappers belonging to it.
- Editor selection and other stored wrappers must be cleared before replacing or closing their scene.
- Scene-owning members are declared before non-owning entity wrappers so wrappers are destroyed first.
